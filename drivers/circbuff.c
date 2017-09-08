
#include "circbuff.h"

void cb_init(CircularBuffer* cb, uint8_t* buf, uint32_t size)
{
    cb->head = 0;
    cb->tail = 0;
    cb->mask = size - 1;
    cb->buf = buf;
    return;
}

int cb_putc(CircularBuffer* cb, uint8_t c)
{
    uint32_t next_head = (cb->head + 1) & cb->mask;
    if (next_head == cb->tail){
        // buff full!
        return -1;
    }
    cb->buf[cb->head] = c;
    cb->head = next_head;
    return 0;
}

uint8_t cb_getc(CircularBuffer* cb)
{
    if (cb->head == cb->tail){
        // No data
        return 0;
    }

    uint32_t next_tail = (cb->tail + 1) & cb->mask;
    uint8_t c = cb->buf[cb->tail];
    cb->tail = next_tail;
    return c;
}

// if head + 1 == tail, then full
int cb_isfull(CircularBuffer* cb)
{
    uint32_t next = (cb->head + 1) & cb->mask;
    return next == cb->tail;
}

// if head == tail, then no data
int cb_isempty(CircularBuffer* cb)
{
    return cb->head == cb->tail;
}

