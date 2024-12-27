#include <cstdio>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/i2c.h>
#include <hardware/timer.h>
#include <hardware/vreg.h>
#include <hardware/clocks.h>

#include <lwip/tcp.h>
#include <lwip/ip_addr.h>
#include <lwip/dns.h>
#include <lwip/netif.h>
#include <lwip/timeouts.h>

#include <encoder.hpp>
#include <button.hpp>
#include <pico_keypad.hpp>
#include <u8g2.h>
#include "u8g2functions.h"
#include <DCCEXProtocol.h>

#include "wifi.h"
#include "wifi_connection.h"
#include "DCCDelegate.h"
#include "print_stream.h"

/*
A demonstration of a rotary encoder being used to control a value.
*/
DCCExController::DCCEXProtocol dccexProtocol;
MyDelegate dccDelegate;
PrintStream printStream;
bool dccexProtocolConnected = false;

// using namespace encoder;

#define OTHER_LED 27

// Create an encoder on the 3 ADC pins, using PIO 0 and State Machine 0
const uint PIN_A = 20;         // The A channel pin
const uint PIN_B = 21;         // The B channel pin
const uint PIN_C = PIN_UNUSED;  // The common pin
encoder::Encoder enc(pio0, 0, {PIN_A, PIN_B}, PIN_C);

// The min and max value
const int MIN_VALUE = 0;
const int MAX_VALUE = 100;

int value = 50;
int old_key = -1;

const int ROW_NUM = 4;    // four rows
const int COLUMN_NUM = 4; // four columns

uint8_t pin_column[COLUMN_NUM] = {6, 7, 8, 9};
uint8_t pin_rows[ROW_NUM] = {10, 11, 12, 13};

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

u8g2_t u8g2;

void setup_display()
{
    // Initialize the U8g2 library with an I2C interface
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay_hw_i2c);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0); // Wake up the display
}

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif


volatile bool timer_fired = false;
uint64_t timer_period_us = 2 * 60 * 1000000;  // 2 minutes in microseconds

// Timer callback function
bool timer_callback(repeating_timer_t *rt) {
    timer_fired = true;
    return false;  // Return false to stop the repeating timer
}

// Perform initialisation
int pico_led_init(void)
{
    gpio_init(OTHER_LED);
    gpio_set_dir(OTHER_LED, GPIO_OUT);

#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on)
{
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

// Variable to track the LED state
bool led_state = false;
bool led_toggle = false;

// Timer callback function to toggle the LED
bool toggle_led_callback(repeating_timer_t *rt)
{
    gpio_put(OTHER_LED, led_state);
    led_state = !led_state; // Toggle LED state
    led_toggle = true;
    return true; // Keep repeating the timer
}

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

// Callback function for connection establishment
static err_t connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        printf("Connected to server\n");
        TCPSocketStream *stream = new TCPSocketStream(tpcb);
        // Now you can use the stream to read/write data
        dccexProtocol.connect(stream);
        dccexProtocol.enableHeartbeat();
        dccexProtocol.requestServerVersion();
        dccexProtocolConnected  = true;
    } else {
        printf("Connection failed\n");
        tcp_close(tpcb);
    }
    return err;
}

// Function to initiate a connection to the server
void connect_to_server(const char *server_ip, uint16_t port) {
    ip_addr_t server_addr;
    if (!ipaddr_aton(server_ip, &server_addr)) {
        printf("Invalid IP address\n");
        return;
    }

    struct tcp_pcb *pcb = tcp_new();
    if (pcb == nullptr) {
        printf("Failed to create PCB\n");
        return;
    }

    tcp_connect(pcb, &server_addr, port, connect_callback);
}

int main()
{

    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    setup_display();

    //  pico_keypad_init(columns, rows, matrix);
    PicoKeypad keypad = PicoKeypad(MAKE_KEYMAP(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
    Button button = Button(14, Polarity::ACTIVE_LOW, 0);
    // Sleep 8 seconds to give enough time to connect up a terminal
    // sleep_ms(8000);

    // Uncomment the below line to reverse the counting direction
    // enc.direction(REVERSED_DIR);

    // Initialise the encoder
    enc.init();

    repeating_timer_t timer;
    add_repeating_timer_ms(1000, toggle_led_callback, NULL, &timer); // 500ms interval


// Initialize the repeating timer
    repeating_timer_t timer_display;
    add_repeating_timer_us(-timer_period_us, timer_callback, NULL, &timer_display);


    char str_value[20] = "";
    char key_value[20] = "";
    char button_value[20] = "";


    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...%s\n",WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }
    dccexProtocol.setLogStream(&printStream);
    dccexProtocol.setDelegate(&dccDelegate);

    connect_to_server("192.168.1.227", 2560);

    absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;

    // Loop forever
    while (true)
    {
        if(led_toggle){
            led_toggle = false;
            pico_set_led(led_state);
        }
    

        bool updated = false;
        int32_t delta = enc.delta();
        if (delta != 0)
        {
            if (delta > 0)
                value = MIN(value + 1, MAX_VALUE);
            else
                value = MAX(value - 1, MIN_VALUE);

            // Print out the new value
            printf("Value = %d\n", value);
            snprintf(str_value, 20, "Value: %d", value);
            updated = true;
        }

        auto key = keypad.getKey();

        if (key)
        {
            printf("Key pressed: %c\n", key);
            snprintf(key_value, 20, "Last Key: %c", key);
            updated = true;
        }

        if (button.read())
        {
            printf("Button Pressed\n");
            snprintf(button_value, 20, "Button at : %d", millis());
            updated = true;
        }

        if(updated)
        {
            u8g2_SetPowerSave(&u8g2, 0); // Wake up the display
            cancel_repeating_timer(&timer_display);
            add_repeating_timer_us(-timer_period_us, timer_callback, NULL, &timer_display);
            // printf("Timer reset\n");
        }

        if(timer_fired)
        {
            u8g2_SetPowerSave(&u8g2, 1); // Wake up the display
            printf("Timer fired after 2 minutes!\n");
            timer_fired = false;  // Reset the flag

            // Restart the timer if needed
            //add_repeating_timer_us(-timer_period_us, timer_callback, NULL, &timer_display);
        }

        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_6x13_tr);     // Select font
        u8g2_DrawStr(&u8g2, 0, 20, "Hello, U8g2!"); // Draw a string
        u8g2_DrawStr(&u8g2, 0, 30, str_value);      // Draw a string
        u8g2_DrawStr(&u8g2, 0, 40, key_value);      // Draw a string
        u8g2_DrawStr(&u8g2, 0, 50, button_value);   // Draw a string
        u8g2_SendBuffer(&u8g2);

        sys_check_timeouts();

        if(dccexProtocolConnected)
        {
        }
        dccexProtocol.check();
    }
}
