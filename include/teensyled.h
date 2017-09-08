/* led.h
 * Helpers for debug LED on teensy3 pin 13 (PC5)
 * Patrick Schubert September 2017
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
