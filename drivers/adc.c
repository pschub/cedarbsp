/*
    adc.c - support for adc peripheral on mk20/teensy

    This file contains ADC initialization and calibration, a simple blocking
    ADC read function, and a conversion function for reading the internal
    temperature sensor.


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

#include "kinetis.h"
#include "adc.h"

// Slope and offset for internal temp sensor, based on datasheet.
#define TEMP_SLOPE (583.0903790087464)
#define VTEMP_25   (0.719)

static int adc_calibrate(void);

// Initialize and calibrate ADC0 and ADC1.
// With default config, ADC converts in 12us.
// Returns -1 if calibration failed (unlikely?), otherwise 0.
int adc_init(void)
{
    // Enable clocks
    SIM_SCGC3 |= SIM_SCGC3_ADC1;
    SIM_SCGC6 |= SIM_SCGC6_ADC0 | SIM_SCGC6_PDB;

    if (adc_calibrate()){
        // Cal failed. :(
        return -1;
    }

    // Use oscerclk (16mhz) div 4 = 4Mhz
    ADC0_CFG1 = ADC_CFG1_ADIV(2) | ADC_CFG1_ADICLK(2);
    ADC1_CFG1 = ADC_CFG1_ADIV(2) | ADC_CFG1_ADICLK(2);

    // 16-bit mode, long sample time.
    ADC0_CFG1 |= ADC_CFG1_MODE(0x3) | ADC_CFG1_ADLSMP;
    ADC1_CFG1 |= ADC_CFG1_MODE(0x3) | ADC_CFG1_ADLSMP;

    // +20 extra sample ticks
    ADC0_CFG2 = ADC_CFG2_ADLSTS(0x0);
    ADC1_CFG2 = ADC_CFG2_ADLSTS(0x0);

    // Enable bandgap buffer for reading 1V reference
    PMC_REGSC |= PMC_REGSC_BGBE;

    // Enable adc interrupt
    NVIC_ENABLE_IRQ(IRQ_ADC0);
    NVIC_SET_PRIORITY(IRQ_ADC0, 65);

    return 0;
}


// Run calibration routine on ADC
static int adc_calibrate(void)
{
    ADC1_SC1A = ADC_SC1_ADCH(0x1F);
    ADC1_SC1B = ADC_SC1_ADCH(0x1F);
    ADC0_SC1A = ADC_SC1_ADCH(0x1F);
    ADC0_SC1B = ADC_SC1_ADCH(0x1F);

    ADC1_SC2 = 0;
    ADC1_SC3 = 0;
    ADC1_CFG1 = 0;
    ADC1_CFG2 = 0;
    ADC1_PGA = 0;

    ADC0_SC2 = 0;
    ADC0_SC3 = 0;
    ADC0_CFG1 = 0;
    ADC0_CFG2 = 0;
    ADC0_PGA = 0;

    // hardware average max
    ADC0_SC3 = ADC_SC3_AVGE | ADC_SC3_AVGS(3);
    ADC1_SC3 = ADC_SC3_AVGE | ADC_SC3_AVGS(3);

    // Use oscerclk (16mhz) div 8 = 2Mhz
    ADC0_CFG1 = ADC_CFG1_ADIV(3) | ADC_CFG1_ADICLK(2);
    ADC1_CFG1 = ADC_CFG1_ADIV(3) | ADC_CFG1_ADICLK(2);

    // long sample time
    ADC0_CFG1 |= ADC_CFG1_ADLSMP;
    ADC1_CFG1 |= ADC_CFG1_ADLSMP;

    // start cal
    ADC0_SC3 = ADC_SC3_CAL;
    ADC1_SC3 = ADC_SC3_CAL;

    while (ADC0_SC3 & ADC_SC3_CAL);
    while (ADC1_SC3 & ADC_SC3_CAL);

    uint32_t ad0_failed = (ADC0_SC3 & ADC_SC3_CALF);
    uint32_t ad1_failed = (ADC1_SC3 & ADC_SC3_CALF);

    if (ad0_failed || ad1_failed)
    {   // Failed!
        return -1;
    }

    uint16_t cal;
    // Calculate and set CH0 positive
    cal = ADC0_CLP0 + ADC0_CLP1 + ADC0_CLP2 + ADC0_CLP3 + ADC0_CLP4 + ADC0_CLPS;
    cal = (cal >> 1) | (1<<15);
    ADC0_PG = cal;

    // Calculate and set CH0 negative
    cal = ADC0_CLM0 + ADC0_CLM1 + ADC0_CLM2 + ADC0_CLM3 + ADC0_CLM4 + ADC0_CLMS;
    cal = (cal >> 1) | (1<<15);
    ADC0_MG = cal;

    // Calc and set CH1 positive
    cal = ADC1_CLP0 + ADC1_CLP1 + ADC1_CLP2 + ADC1_CLP3 + ADC1_CLP4 + ADC1_CLPS;
    cal = (cal >> 1) | (1<<15);
    ADC1_PG = cal;

    // Calculate and set CH1 minus
    cal = ADC1_CLM0 + ADC1_CLM1 + ADC1_CLM2 + ADC1_CLM3 + ADC1_CLM4 + ADC1_CLMS;
    cal = (cal >> 1) | (1<<15);
    ADC1_MG = cal;

    return 0;
}


// Perform a blocking ADC read. Returns results immediately.
uint16_t adc_readone(uint32_t ch)
{
    // Set mux for a/b channel
    if (ch & ADC_B_CHANNEL){
        ADC0_CFG2 |= ADC_CFG2_MUXSEL;
    } else {
        ADC0_CFG2 &= ~(ADC_CFG2_MUXSEL);
    }

    // set software trigger
    ADC0_SC2 &= ~(ADC_SC2_ADTRG);

    ADC0_SC1A = ADC_SC1_ADCH(ch);
    while (ADC0_SC2 & ADC_SC2_ADACT);
    return ADC0_RA;
}


// Given a 16-bit ADC val, return temperature.
float adc_calc_temp(uint16_t val)
{
    return 25.0 - (((val * VOLTS_PER_COUNT)-VTEMP_25)*TEMP_SLOPE);
}



