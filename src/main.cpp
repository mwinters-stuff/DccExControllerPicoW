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

#include <DCCEXProtocol.h>
#include "SingletonHolder.h"
#include "ExternalSingletons.h"
#include "wifi.h"


#define OTHER_LED 27


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

int main()
{

    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);


    repeating_timer_t timer;
    add_repeating_timer_ms(1000, toggle_led_callback, NULL, &timer); // 500ms interval


// Initialize the repeating timer
    repeating_timer_t timer_display;
    add_repeating_timer_us(-timer_period_us, timer_callback, NULL, &timer_display);

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...%s\n",WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }

    SingletonHolder singletonHolder;
    singletonHolder.setup();

    // Loop forever
    while (true)
    {
        singletonHolder.loop();
        if(led_toggle){
            led_toggle = false;
            pico_set_led(led_state);
        }
    

     
        sys_check_timeouts();

    }
}
