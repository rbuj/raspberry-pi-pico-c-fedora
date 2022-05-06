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

typedef enum {
    DPY_THOUSANDS = 0,
    DPY_HUNDREDS,
    DPY_TENS,
    DPY_UNITS,
    DPY_NUM
} DisplaySelect;

const uint data_pin = 18;   // Pin connected to DS of 74HC595
const uint latch_pin = 20;  // Pin connected to ST_CP of 74HC595
const uint clock_pin = 21;  // Pin connected to SH_CP of 74HC595
const uint button_pin = 13; // Pin connected to button
const uint display[DPY_NUM] = { 17, 16, 15, 14 }; // Pins to select the display of 3461BS

DisplaySelect display_com = DPY_THOUSANDS;
uint8_t number[DPY_NUM] = { 0, 0, 0, 0 };
uint counter = 0;
bool chronometer_stopped = true;

/**
 * The function below is a modified version of shiftOut:
 * void shiftOut(pin_size_t dataPin, uint8_t clockPin, BitOrder bitOrder, uint8_t val)
 * https://github.com/arduino/ArduinoCore-mbed/blob/master/cores/arduino/wiring_shift.cpp
 * SPDX-License-Identifier: LGPL-2.1+ (deprecated as of 2.0rc2)
 */
void shift_out(BitOrder bit_order, unsigned char value) {
    for (uint8_t i = 0; i < CHAR_BIT; i++)  {
        if (bit_order == LSB_FIRST)
            gpio_put(data_pin, !!(value & (1 << i)) ? true : false);
        else
            gpio_put(data_pin, !!(value & (1 << (7 - i))) ? true : false);
        gpio_put(clock_pin, true);
        gpio_put(clock_pin, false);
    }
}

void write_data(unsigned char value) {
    // Make latchPin output low level
    gpio_put(latch_pin, false);
    // Send serial data to 74HC595
    shift_out(LSB_FIRST, value);
    // Make latchPin output high level, then 74HC595 will update the data to parallel output
    gpio_put(latch_pin, true);
}

void select_display(DisplaySelect d) {
    for (DisplaySelect i = 0; i < DPY_NUM; i++) {
        gpio_put(display[i], false);
    }
    gpio_put(display[d], true);
}

bool timer_callback(repeating_timer_t *rt) {
    (void)rt;
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

    write_data(UCHAR_MAX); // Clear the display content

    select_display(display_com);
    write_data((display_com == DPY_TENS) || (display_com == DPY_THOUSANDS) ?
               bits[number[display_com]] | 0x80 :
               bits[number[display_com]]); // Send data to 74HC595

    if (!chronometer_stopped) {
        counter++;
        if (counter == 20) {
            counter = 0;
            number[DPY_UNITS]++;
        }

        if (number[DPY_UNITS] == 10) {
            number[DPY_UNITS] = 0;
            number[DPY_TENS]++;
        }

        if (number[DPY_TENS] == 10) {
            number[DPY_TENS] = 0;
            number[DPY_HUNDREDS]++;
        }

        if (number[DPY_HUNDREDS] == 6) {
            number[DPY_HUNDREDS] = 0;
            number[DPY_THOUSANDS]++;
        }

        if (number[DPY_THOUSANDS] == 10) {
            number[DPY_THOUSANDS] = 0;
        }
    }

    if (display_com < DPY_UNITS)
        display_com++;
    else
        display_com = 0;

    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    (void)gpio;
    if (!(events & GPIO_IRQ_EDGE_FALL))
        return;
    if (chronometer_stopped) {
        for (DisplaySelect i = 0; i < DPY_NUM; i++) {
            number[i] = 0;
        }
    }
    chronometer_stopped = !chronometer_stopped;
}

int main(void) {
    repeating_timer_t timer;

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

    // Initialize button pin
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);

    for (DisplaySelect i = 0; i < DPY_NUM; i++) {
        gpio_init(display[i]);
        gpio_set_dir(display[i], GPIO_OUT);
    }

    // Initialize chosen serial port
    stdio_init_all();

    // Repeat the task of writing to a display every 5 ms
    add_repeating_timer_ms(5, timer_callback, NULL, &timer);

    gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_FALL , true, &gpio_callback);

    // Loop forever
    while (true) {
        tight_loop_contents();
    }

    return EXIT_SUCCESS;
}
