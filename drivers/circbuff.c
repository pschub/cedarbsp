/*
    circbuff.c - circular buffer library

    Provides simple byte queue of arbitrary length. User supplies the storage
    for the buffer, which must be sized as a power of 2.


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

#include "circbuff.h"

// Initialize buf as empty.
void cb_init(CircularBuffer* cb, uint8_t* buf, uint32_t size)
{
    cb->head = 0;
    cb->tail = 0;
    cb->mask = size - 1;
    cb->buf = buf;
    return;
}

// Put one byte in the buffer.
// Return -1 if full, 0 on success.
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

// Dequeue one byte from the buffer.
// Return 0 if empty.
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

// Return true if buf is full.
int cb_isfull(CircularBuffer* cb)
{
    // if head + 1 == tail, then full
    uint32_t next = (cb->head + 1) & cb->mask;
    return next == cb->tail;
}

// Return true if buf is empty.
int cb_isempty(CircularBuffer* cb)
{
    // if head == tail, then no data
    return cb->head == cb->tail;
}

// Return amount of empty space.
uint32_t cb_space(CircularBuffer* cb)
{
    uint32_t tail = cb->tail;
    uint32_t head = cb->head;
    if (head >= tail){
        return (cb->mask + 1) - head + tail;
    } else {
        return (cb->mask + 1) - tail + head;
    }
}

