/*
    circbuff.h - circular buffer library

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
