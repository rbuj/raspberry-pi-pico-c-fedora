/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning this example requires a board with a regular LED
#else
const uint led_pin = PICO_DEFAULT_LED_PIN;

bool timer_callback(repeating_timer_t *rt) {
    (void)rt;
    static bool led_stat = true;
    gpio_put(led_pin, led_stat);
    led_stat = !led_stat;
    return true;
}
#endif

int main(void) {
#ifdef PICO_DEFAULT_LED_PIN
    repeating_timer_t timer;

    stdio_init_all();

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);

    // Loop forever
    while (true) {
        tight_loop_contents();
    }
#endif
    return EXIT_SUCCESS;
}
