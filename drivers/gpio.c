/* gpio.c
 * Interface to GPIO on teensy3. Uses teensy pin numbers for id-ing port+pin.
 * Offers basic init, input/output, and set/clr/toggle. Supports OR-ing
 * TEENSY_PINs together to affect multiple pins simultaneously. THIS ONLY WORKS
 * FOR TEENSY_PINS ON THE SAME PORT!!! OR-ing TEENSY_PINs on different ports
 * (such as TEENSY_PIN_0 and TEENSY_PIN_9) will result in "interesting" results.
 * Patrick Schubert 2017
 */
#include "kinetis.h"
#include "gpio.h"

// Macros to get the correct register based on the port (and pin)
#define PCR_PORT(port,bit)    (*(volatile uint32_t *)(0x40049000 + port*0x1000 + bit*4))
#define OUT_PORT(n)    (*(volatile uint32_t *)(0x400FF000 + n*0x40))
#define SET_PORT(n)    (*(volatile uint32_t *)(0x400FF004 + n*0x40))
#define CLR_PORT(n)    (*(volatile uint32_t *)(0x400FF008 + n*0x40))
#define TOGGLE_PORT(n) (*(volatile uint32_t *)(0x400FF00C + n*0x40))
#define IN_PORT(n)     (*(volatile uint32_t *)(0x400FF010 + n*0x40))
#define DIR_PORT(n)    (*(volatile uint32_t *)(0x400FF014 + n*0x40))

// gpio_init only supports config of the first byte of PCR.
#define PCR_MASK (0xFF)

// Macros to separate the port/bit of a TEENSY_PIN
#define BITS_BITMASK (0x3FFFF)
#define PORT_BITMASK (0x3<<24)


void gpio_init(uint32_t teensy_pins, uint32_t config)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);
    uint32_t pcr_config = PORT_PCR_MUX(0x1) | (config & PCR_MASK);

    // Set pin config reg
    uint32_t bit_number = 0x0;
    while (bits){
        if (bits & 0x1){
            PCR_PORT(port, bit_number) = pcr_config;
        }
        bits = bits >> 1;
        bit_number++;
    }

    // Set direction
    if (GPIO_OUT & config){
        DIR_PORT(port) |= teensy_pins & BITS_BITMASK;
    } else {
        DIR_PORT(port) &= ~(teensy_pins & BITS_BITMASK);
    }

    return;
}

void gpio_set(uint32_t teensy_pins)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);

    SET_PORT(port) = bits;
    return;
}

void gpio_clr(uint32_t teensy_pins)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);

    CLR_PORT(port) = bits;
    return;
}

void gpio_toggle(uint32_t teensy_pins)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);

    TOGGLE_PORT(port) = bits;
    return;
}

uint32_t gpio_getvalue(uint32_t teensy_pins)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);

    return (IN_PORT(port) & bits);
}

void gpio_setvalue(uint32_t teensy_pins, uint32_t val)
{
    uint32_t port = (PORT_BITMASK & teensy_pins) >> 24;
    uint32_t bits = (BITS_BITMASK & teensy_pins);

    uint32_t x = OUT_PORT(port) & ~(bits);
    OUT_PORT(port) = x;
    return;
}


