
#include "systick.h"
volatile uint32_t systick_ms;

void systick_isr(void)
{
    systick_ms++;
}

