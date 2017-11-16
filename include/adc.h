/*
    adc.h - support for adc peripheral on mk20/teensy

    This file contains ADC initialization and calibration, a simple blocking
    ADC read function, and a conversion function for reading the internal
    temperature sensor.

    The ADC channel numbers use the Teensy 3.2 numbering for user-friendliness.

    
    This file is part of Cedar BSP, a bsp library for Teensy3.2 and similar.
    Copyright 2017 Patrick Schubert

    Cedar BSP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cedar BSP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ADC_H_FILE
#define ADC_H_FILE

#include <stdint.h>

// For a channel that uses B side of mux.
#define ADC_B_CHANNEL (1<<9)

// ADC Channels
#define ADC_AD0     (0x5 | ADC_B_CHANNEL)
#define ADC_AD1     (0xE)
#define ADC_AD2     (0x8)   // ADC0/1 capable
#define ADC_AD3     (0x9)   // ADC0/1 capable
#define ADC_AD4     (0xD)
#define ADC_AD5     (0xC)
#define ADC_AD6     (0x6 | ADC_B_CHANNEL)
#define ADC_AD7     (0x7 | ADC_B_CHANNEL)
#define ADC_AD8     (0xF)
#define ADC_AD9     (0x4 | ADC_B_CHANNEL)
#define ADC_TEMP    (0x1A)
#define ADC_BANDGAP (0x1B)
#define ADC_OFF     (0x1F)

#define VOLTS_PER_COUNT (5.035400390625e-05)
#define ADC_TO_VOLTS(n) (VOLTS_PER_COUNT*((float)n))

#define ADC_CH_MASK (0x1F)

// Initialize and calibrate ADC0 and ADC1.
// With default config, converts in 12us.
int adc_init(void);

// Perform a blocking ADC read using ADC0.
// Returns results immediately.
uint16_t adc_readone(uint32_t ch);

// Given a 16-bit ADC val, return temperature.
float adc_calc_temp(uint16_t val);

#endif // ADC_H_FILE
