
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

typedef struct {
    volatile uint32_t head;
    volatile uint32_t tail;
    uint32_t mask;
    volatile uint8_t* buf;
} CircularBuffer;

void cb_init(CircularBuffer* cb, uint8_t* buf, uint32_t size);
int cb_putc(CircularBuffer* cb, uint8_t c);
uint8_t cb_getc(CircularBuffer* cb);
int cb_isfull(CircularBuffer* cb);
int cb_isempty(CircularBuffer* cb);
uint32_t cb_space(CircularBuffer* cb);

#endif
