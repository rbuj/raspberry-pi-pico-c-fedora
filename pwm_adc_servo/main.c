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
#include "hardware/clocks.h"
#include "hardware/pwm.h"

int main(void) {
    const uint servo_pin = 16;
    const uint pwm_freq = 50; // 50hz, 20ms
    uint slice = pwm_gpio_to_slice_num(servo_pin);
    uint channel = pwm_gpio_to_channel(servo_pin);
    pwm_config cfg = pwm_get_default_config();
    // Vref = 3.3 V, 12-bit ADC (4096 levels)
    const float conversion_factor = 2000.0f / 4096.0f;

    // Initialize chosen serial port
    stdio_init_all();

    // Initialize ADC0
    adc_init();
    adc_gpio_init(26); // Allowable GPIO numbers are 26 to 29 inclusive
    adc_select_input(0); // 0...3 are GPIOs 26...29 respectively.
                         // Input 4 is the onboard temperature sensor.

    // Initialize PWM
    gpio_set_function(servo_pin, GPIO_FUNC_PWM);
    pwm_config_set_wrap(&cfg, 20000);
    pwm_config_set_clkdiv(&cfg, (float)(clock_get_hz(clk_sys)) / (pwm_freq*20000));
    pwm_init(pwm_gpio_to_slice_num(servo_pin), &cfg, true);

    // Loop forever
    while (true) {
        uint16_t potentiometer = adc_read();
        uint16_t level = 500 + (uint16_t)(conversion_factor * (float) potentiometer);
        pwm_set_chan_level(slice, channel, level);
        printf("Pot: %05u, Angle: %.0f\n",
               potentiometer, (180.0f / 4096.0f) * (float) potentiometer);
        sleep_ms(60);
    }
    return EXIT_SUCCESS;
}
