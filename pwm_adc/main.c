/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

int main(void) {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint led_pin = PICO_DEFAULT_LED_PIN;
    pwm_config config = pwm_get_default_config();
    uint16_t raw, temp, aux;

    // Initialize chosen serial port
    stdio_init_all();

    // Initialize ADC0
    adc_init();
    adc_gpio_init(26); // Allowable GPIO numbers are 26 to 29 inclusive
    adc_select_input(0); // 0...3 are GPIOs 26...29 respectively.
                         // Input 4 is the onboard temperature sensor.

    // Initialize PWM
    gpio_set_function(led_pin, GPIO_FUNC_PWM);
    pwm_set_gpio_level(led_pin, 0);
    pwm_init(pwm_gpio_to_slice_num(led_pin), &config, true);

    raw = adc_read();
    temp = aux = raw >> 4;
    pwm_set_gpio_level(led_pin, temp*temp);

    // Loop forever
    while (true) {
        if (aux != temp) {
            temp = aux;
            pwm_set_gpio_level(led_pin, temp*temp);
        }
        // Square the fade value to make the LED's brightness appear more linear
        printf("Raw value: %04u, %03u\n", raw, temp);
        sleep_ms(100);
        raw = adc_read();
        aux = raw >> 4;
    }
#endif
    return EXIT_SUCCESS;
}
