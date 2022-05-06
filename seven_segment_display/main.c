/**
 * Copyright (c) 2005-2006 David A. Mellis
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

typedef enum {
    LSB_FIRST = 0,
    MSB_FIRST,
} BitOrder;

/**
 * The function below is a modified version of shiftOut:
 * void shiftOut(pin_size_t dataPin, uint8_t clockPin, BitOrder bitOrder, uint8_t val)
 * https://github.com/arduino/ArduinoCore-mbed/blob/master/cores/arduino/wiring_shift.cpp
 * SPDX-License-Identifier: LGPL-2.1+ (deprecated as of 2.0rc2)
 */
void shift_out(uint data_pin, uint clock_pin, BitOrder bit_order, unsigned char value) {
    for (uint8_t i = 0; i < CHAR_BIT; i++)  {
        if (bit_order == LSB_FIRST)
            gpio_put(data_pin, !!(value & (1 << i)) ? true : false);
        else
            gpio_put(data_pin, !!(value & (1 << (7 - i))) ? true : false);
        gpio_put(clock_pin, true);
        gpio_put(clock_pin, false);
    }
}

void write_data(uint data_pin, uint clock_pin, uint latch_pin, unsigned char value) {
    // Make latchPin output low level
    gpio_put(latch_pin, false);
    // Send serial data to 74HC595
    shift_out(data_pin, clock_pin, LSB_FIRST, value);
    // Make latchPin output high level, then 74HC595 will update the data to parallel output
    gpio_put(latch_pin, true);
}

int main(void) {
    const uint data_pin = 18;  // Pin connected to DS of 74HC595
    const uint latch_pin = 20; // Pin connected to ST_CP of 74HC595
    const uint clock_pin = 21; // Pin connected to SH_CP of 74HC595

    /**
     * Further information is available at https://en.wikipedia.org/wiki/Seven-segment_display
     */
    const unsigned char bits[] = {
        0x3f, // 0
        0x06, // 1
        0x5b, // 2
        0x4f, // 3
        0x66, // 4
        0x6d, // 5
        0x7d, // 6
        0x07, // 7
        0x7f, // 8
        0x67  // 9
    };
    size_t bits_n_elem = sizeof(bits) / sizeof(unsigned char);

    // Initialize data pin
    gpio_init(data_pin);
    gpio_set_dir(data_pin, GPIO_OUT);
    gpio_set_outover(data_pin, GPIO_OVERRIDE_INVERT);

    // Initialize latch pin
    gpio_init(latch_pin);
    gpio_set_dir(latch_pin, GPIO_OUT);

    // Initialize clock pin
    gpio_init(clock_pin);
    gpio_set_dir(clock_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();

    // Loop forever
    while (true) {
        for (size_t i = 0; i < bits_n_elem; i++) {
            printf("%zu\r\n", i);
            write_data(data_pin, clock_pin, latch_pin, bits[i]); // Send data to 74HC595
            sleep_ms(1000); // delay 1 second
            write_data(data_pin, clock_pin, latch_pin, UCHAR_MAX); // Clear the display content
        }
    }

    return EXIT_SUCCESS;
}
