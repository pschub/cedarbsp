
#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
extern volatile uint32_t systick_ms;
void systick_isr(void);

#endif //SYSTICK_H
