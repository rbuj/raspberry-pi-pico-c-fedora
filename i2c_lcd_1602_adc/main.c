/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

/**
 * DB7 DB6 DB5 DB4 : K E RW RS
 */

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x20
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// 4 LSB of PCF8574
#define LCD_BACKLIGHT_BIT 1U << 3
#define LCD_ENABLE_BIT    1U << 2
#define LCD_RW_BIT        1U << 1
#define LCD_RS_BIT        1U << 0

#define LCD_CHARACTER     1
#define LCD_COMMAND       0

#define lcd_send_command(X) lcd_send_byte((X), LCD_COMMAND)
#define lcd_send_char(X) lcd_send_byte((X), LCD_CHARACTER)
#define lcd_clear() lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND)

#define LCD_1ST_LINE 0
#define LCD_2ND_LINE 1

#define I2C_LCD_MODULE i2c1
#define I2C_LCD_ADDR 0x27
#define I2C_LCD_SDA_PIN 6
#define I2C_LCD_SCL_PIN 7

const bool backlight = true;

void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(I2C_LCD_MODULE, I2C_LCD_ADDR, &val, 1, false);
}

void lcd_send_nibble(uint8_t val) {
    // Address setup time > 60 us
    i2c_write_byte(val);
    sleep_us(100);

    // E cicle time > 1000us
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(600);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(600);
}

/**
 * Each of the 8-bit LCD character or command (D7:0) is sent in two transfers.
 * In each of the transfers, the 4 most significant bits are for data and the
 * 4 least significant bits are for control signals.
 */
void lcd_send_byte(uint8_t val, bool is_char) {
    uint8_t high;
    uint8_t low;

    high = val & 0xF0; /* D7, D6, D5, D4 bits */
    high = is_char ? high | LCD_RS_BIT : high & ~LCD_RS_BIT; /* RS */
    high = backlight ? high | LCD_BACKLIGHT_BIT : high & ~LCD_BACKLIGHT_BIT; /* K */

    lcd_send_nibble(high);

    low = (val << 4) & 0xF0; /* D3, D2, D1, D0 bits */
    low = is_char ? low | LCD_RS_BIT : low & ~LCD_RS_BIT; /* RS */
    low = backlight ? low | LCD_BACKLIGHT_BIT : low & ~LCD_BACKLIGHT_BIT; /* K */

    lcd_send_nibble(low);
}

void lcd_set_cursor(uint8_t line, uint8_t position) {
    uint8_t val = (line == LCD_1ST_LINE) ? 0x80 + position : 0xC0 + position;
    lcd_send_command(val);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_send_char(*s++);
    }
}

void lcd_init(void) {
    lcd_send_command(0x03);
    lcd_send_command(0x03);
    lcd_send_command(0x03);
    lcd_send_command(0x02);

    lcd_send_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
    lcd_send_command(LCD_FUNCTIONSET | LCD_2LINE);
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
    lcd_clear();
}

int main(void) {
    stdio_init_all();

    // LCD via i2c, using I2C1 module on the GP6 (SDA), GP7 (SCL) pins
    i2c_init(I2C_LCD_MODULE, 100 * 1000);
    gpio_set_function(I2C_LCD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_LCD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_LCD_SDA_PIN);
    gpio_pull_up(I2C_LCD_SCL_PIN);

    adc_init();
    adc_gpio_init(26); // Allowable GPIO numbers are 26 to 29 inclusive

    lcd_init();

    adc_select_input(0); // 0...3 are GPIOs 26...29 respectively.
                         // Input 4 is the onboard temperature sensor.
    while (true) {
#define TEMP_BUFFER_SIZE 16
        // Vref = 3.3 V, 12-bit ADC (4096 levels)
        const float conversion_factor = 3.3f / 4096.0f;
        char char_buffer[TEMP_BUFFER_SIZE];
        uint16_t raw = adc_read();
        float volts = conversion_factor * (float) raw;

        // print to LCD
        lcd_set_cursor (LCD_1ST_LINE, 0);
        snprintf(char_buffer, TEMP_BUFFER_SIZE, "Raw: %04u", raw);
        lcd_string(char_buffer);

        lcd_set_cursor (LCD_2ND_LINE, 0);
        snprintf(char_buffer, TEMP_BUFFER_SIZE, "Voltage: %.2f V", volts);
        lcd_string(char_buffer);

        // print to Serial Port
        printf("Raw value: %04u, voltage: %.2f V\n", raw, volts);

        sleep_ms(1000);
#undef TEMP_BUFFER_SIZE
    }

    return EXIT_SUCCESS;
}
