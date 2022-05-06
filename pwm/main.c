/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

int main(void) {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint led_pin = PICO_DEFAULT_LED_PIN;
    const uint pwm_freq = 2000; // 2khz
    uint slice = pwm_gpio_to_slice_num(led_pin);
    uint channel = pwm_gpio_to_channel(led_pin);
    pwm_config cfg = pwm_get_default_config();
    uint16_t i;

    // Initialize chosen serial port
    stdio_init_all();

    // Initialize PWM
    gpio_set_function(led_pin, GPIO_FUNC_PWM); // setup pwm pin
    pwm_config_set_clkdiv(&cfg, (float)(clock_get_hz(clk_sys)) / (pwm_freq*0xffffu));
    pwm_init(slice, &cfg, true);
    pwm_set_chan_level(slice, channel, 0); // set initial output value

    printf("System clock speed is %luhz\n", clock_get_hz(clk_sys));
    // Loop forever
    while (true) {
        for (i = 0; i < 0xff; i++) {
            pwm_set_chan_level(slice, channel, i*i);
            sleep_ms(5);
        }
        for (i = 0xff; i != 0; i--) {
            pwm_set_chan_level(slice, channel, i*i);
            sleep_ms(5);
        }
    }
#endif
    return EXIT_SUCCESS;
}
