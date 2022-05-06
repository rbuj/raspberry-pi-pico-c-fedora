/**
 * Copyright (c) 2022 Robert Buj <robert.buj@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main(void) {
    stdio_init_all();

    adc_init();
    adc_gpio_init(26); // Allowable GPIO numbers are 26 to 29 inclusive
    adc_select_input(0); // 0...3 are GPIOs 26...29 respectively.
                         // Input 4 is the onboard temperature sensor.

    // Loop forever
    while (true) {
        uint16_t raw = adc_read();
        // conversion factor of 12-bit ADC = Vref / 2¹² = 3.3 V / 4096
        float volts = (3.3f / 4096.0f) * (float) raw;
        printf("Raw value: %04u, voltage: %.2f V\n", raw, volts);
        sleep_ms(1000);
    }

    return EXIT_SUCCESS;
}
