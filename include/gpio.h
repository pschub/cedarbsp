/*
    gpio.h - support for gpio on mk20/teensy

    Uses teensy pin numbers for identifying port+pin. Offers basic init,
    input/output, and set/clr/toggle. Supports OR-ing TEENSY_PINs together to
    affect mltiple pins simultaneously, though THIS ONLY WORKS FOR PINS ON THE
    SAME PORT!!! OR-ing TEENSY_PINs on different ports (such as TEENSY_PIN_0
    and TEENSY_PIN_9) will result in "interesting" results.


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

#ifndef GPIO_H
#define GPIO_H

// MK20 gpio ports //
#define GPIO_PORTA (0)
#define GPIO_PORTB (1)
#define GPIO_PORTC (2)
#define GPIO_PORTD (3)

// Teensy Pins //
#define TEENSY_PIN_0     ((GPIO_PORTB<<24)|(1<<16))
#define TEENSY_PIN_1     ((GPIO_PORTB<<24)|(1<<17))
#define TEENSY_PIN_2     ((GPIO_PORTD<<24)|(1<<0))
#define TEENSY_PIN_3     ((GPIO_PORTA<<24)|(1<<12))
#define TEENSY_PIN_4     ((GPIO_PORTA<<24)|(1<<13))
#define TEENSY_PIN_5     ((GPIO_PORTD<<24)|(1<<7))
#define TEENSY_PIN_6     ((GPIO_PORTD<<24)|(1<<4))
#define TEENSY_PIN_7     ((GPIO_PORTD<<24)|(1<<2))
#define TEENSY_PIN_8     ((GPIO_PORTD<<24)|(1<<3))
#define TEENSY_PIN_9     ((GPIO_PORTC<<24)|(1<<3))
#define TEENSY_PIN_10    ((GPIO_PORTC<<24)|(1<<4))
#define TEENSY_PIN_11    ((GPIO_PORTC<<24)|(1<<6))
#define TEENSY_PIN_12    ((GPIO_PORTC<<24)|(1<<7))
#define TEENSY_PIN_13    ((GPIO_PORTC<<24)|(1<<5))
#define TEENSY_PIN_14    ((GPIO_PORTD<<24)|(1<<1))
#define TEENSY_PIN_15    ((GPIO_PORTC<<24)|(1<<0))
#define TEENSY_PIN_16    ((GPIO_PORTB<<24)|(1<<0))
#define TEENSY_PIN_17    ((GPIO_PORTB<<24)|(1<<1))
#define TEENSY_PIN_18    ((GPIO_PORTB<<24)|(1<<3))
#define TEENSY_PIN_19    ((GPIO_PORTB<<24)|(1<<2))
#define TEENSY_PIN_20    ((GPIO_PORTD<<24)|(1<<5))
#define TEENSY_PIN_21    ((GPIO_PORTD<<24)|(1<<6))
#define TEENSY_PIN_22    ((GPIO_PORTC<<24)|(1<<1))
#define TEENSY_PIN_23    ((GPIO_PORTC<<24)|(1<<2))

// GPIO configuration options
#define GPIO_OUT         (1<<8)
#define GPIO_IN          (0<<8)
#define GPIO_STRONG      PORT_PCR_DSE
#define GPIO_OPENDRAIN   PORT_PCR_ODE
#define GPIO_FILTER      PORT_PCR_PFE
#define GPIO_SLOW        PORT_PCR_SRE
#define GPIO_PULLUP      PORT_PCR_PE
#define GPIO_PULL_EN     PCR_PS


void gpio_init(uint32_t teensy_pins, uint32_t config);
void gpio_set(uint32_t teensy_pins);
void gpio_clr(uint32_t teensy_pins);
void gpio_toggle(uint32_t teensy_pins);
uint32_t gpio_getvalue(uint32_t teensy_pins);
void gpio_setvalue(uint32_t teensy_pins, uint32_t val);

#endif // GPIO_H
