#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <u8g2.h>

// Define I2C pins for Raspberry Pi Pico
#define I2C_PORT i2c0
#define I2C_SDA  4  // Pin GP6
#define I2C_SCL  5  // Pin GP7
#define I2C_BAUDRATE 400000  // 400 kHz

// Function to initialize hardware I2C
void hardware_i2c_init() {
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // printf("Hardware I2C initialized on SDA: %d, SCL: %d\n", I2C_SDA, I2C_SCL);
}

// Hardware I2C implementation for u8x8_byte callback
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[32];
    static uint8_t buf_idx;

    switch (msg) {
        case U8X8_MSG_BYTE_INIT: // Initialize I2C
            hardware_i2c_init();
            return 1;

        case U8X8_MSG_BYTE_START_TRANSFER: // Start I2C transfer
            buf_idx = 0;
            return 1;

        case U8X8_MSG_BYTE_SEND: // Send data via I2C
            while (arg_int > 0) {
                buffer[buf_idx++] = *(uint8_t *)arg_ptr;
                arg_ptr = (uint8_t *)arg_ptr + 1;
                arg_int--;
            }
            return 1;

        case U8X8_MSG_BYTE_END_TRANSFER: { // End transfer and send buffer via hardware I2C
            uint8_t addr = u8x8_GetI2CAddress(u8x8) >> 1; // 7-bit I2C address
            i2c_write_blocking(I2C_PORT, addr, buffer, buf_idx, false);
            return 1;
        }

        case U8X8_MSG_BYTE_SET_DC: // Not used for I2C
            return 1;

        default:
            return 0;
    }
}

// GPIO setup function (not required for I2C but for displays needing reset)
uint8_t u8x8_gpio_and_delay_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: // Initialize GPIO
            return 1;

        case U8X8_MSG_DELAY_MILLI: // Millisecond delay
            sleep_ms(arg_int);
            return 1;

        default:
            return 0;
    }
}
