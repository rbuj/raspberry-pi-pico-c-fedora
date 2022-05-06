/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

void gpio_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        gpio_put(gpio, true);
        printf("LED: ON\r\n");
    }
    if (events & GPIO_IRQ_EDGE_RISE) {
        gpio_put(gpio, false);
        printf("LED: OFF\r\n");
    }
}

int main(void) {
#ifndef PICO_DEFAULT_LED_PIN
#warning this example requires a board with a regular LED
#else
    const uint led_pin = PICO_DEFAULT_LED_PIN;
    const uint button_pin = 13; // Pin connected to button

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize button pin
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);

    // Initialize chosen serial port
    stdio_init_all();

    gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Loop forever
    while (true) {
        tight_loop_contents();
    }
#endif
    return EXIT_SUCCESS;
}
