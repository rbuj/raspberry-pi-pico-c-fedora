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
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); // Input 4 is the onboard temperature sensor

    // Loop forever
    while (true) {
        uint16_t temp_raw = adc_read();
        // Vref = 3.3 V, 12-bit ADC (4096 levels)
        const float conversion_factor = 3.3f / 4096.0f;
        float temp_volts = conversion_factor * (float) temp_raw;
        float temp_degrees = 27.0f - (temp_volts - 0.706f) / 0.001721f;
        printf("Temperature: %.1f C, raw value: 0x%03x, voltage: %f V\n",
               temp_degrees, temp_raw, temp_volts);
        sleep_ms(1000);
    }

    return EXIT_SUCCESS;
}
