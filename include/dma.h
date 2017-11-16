/*
    dma.h - support for dma peripheral on mk20/teensy


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
#ifndef DMA_H_FILE
#define DMA_HFILE

#include <stdint.h>

// DMA Transfer Control Descriptor
typedef struct{
    volatile void* source; // source/dest addresses
    volatile void* dest;

    uint8_t size; // size of transfer (8/16/32 bit)

    int16_t soff; // incr to source/dest per each read/write
    int16_t doff; // not per minor!

    uint16_t nbytes; // number of bytes per minor/major loop
    uint16_t citer;

    int32_t slast; // addr adjust after major loop
    int32_t dlast;

    uint8_t smod;  // modulo
    uint8_t dmod;
} DMA_TCD;

// Initialize DMA system.
void dma_init(void);

// Set DMA mux channel to a specific source.
void dma_set_mux(uint32_t ch, uint32_t source);

// Configure DMA channel with TCD info.
void dma_configure(uint32_t ch, DMA_TCD* tcd);

// Enable DMA channel.
void dma_enable(uint32_t ch);

// Enable DMA interrupt on channel (BYO isr).
void dma_enable_int(uint32_t ch);

#endif // DMA_H_FILE
