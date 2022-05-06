/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

int main(void) {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint led_pin = PICO_DEFAULT_LED_PIN;
    bool led_status = true;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();

    // Loop forever
    while (true) {
        printf("LED: %s\r\n", led_status ? "ON" : "OFF");
        gpio_put(led_pin, led_status);
        sleep_ms(1000);
        led_status = !led_status;
    }
#endif
    return EXIT_SUCCESS;
}
