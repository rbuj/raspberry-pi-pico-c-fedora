/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

int main(void) {
    const uint servo_pin = 16;
    const uint pwm_freq = 50; // 50hz, 20ms
    uint slice = pwm_gpio_to_slice_num(servo_pin);
    uint channel = pwm_gpio_to_channel(servo_pin);
    pwm_config cfg = pwm_get_default_config();
    const float conversion_factor = 2000.0f / 180.0f;
    int i;

    // Initialize chosen serial port
    stdio_init_all();

    // Initialize PWM
    gpio_set_function(servo_pin, GPIO_FUNC_PWM);
    pwm_config_set_wrap(&cfg, 20000);
    pwm_config_set_clkdiv(&cfg, (float)(clock_get_hz(clk_sys)) / (pwm_freq*20000));
    pwm_init(pwm_gpio_to_slice_num(servo_pin), &cfg, true);

    sleep_ms(2000);

    // Loop forever
    while (true) {
        for (i = 0; i < 180; i++) {
            uint16_t level = 500 + (uint16_t)(conversion_factor * (float) i);
            pwm_set_chan_level(slice, channel, level);
            printf("Angle: %d\n", i);
            sleep_ms(40);
        }
        for (i = 180; i > 0; i--) {
            uint16_t level = 500 + (uint16_t)(conversion_factor * (float) i);
            pwm_set_chan_level(slice, channel, level);
            printf("Angle: %d\n", i);
            sleep_ms(40);
        }
    }
    return EXIT_SUCCESS;
}
