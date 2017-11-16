/*
    teensyled.h - support for debug LED on teensy3 pin 13 (PC5)


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

#ifndef TEENSY_LED_H
#define TEENSY_LED_H

/* Helpers for debug LED on teensy pin 13 (PC5) */
#define LED_BIT      (1<<5)

// Initialize LED pin as gpio.
#define LED_INIT()   ( \
                       PORTC_PCR5 = PORT_PCR_MUX(0x1), \
                       GPIOC_PDDR = LED_BIT \
                     )

// Set LED state.
#define LED_ON()     (GPIOC_PSOR=LED_BIT)
#define LED_OFF()    (GPIOC_PCOR=LED_BIT)
#define LED_TOGGLE() (GPIOC_PTOR|=LED_BIT)

#endif // TEENSY_LED_H
