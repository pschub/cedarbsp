
#ifndef DMA_H_FILE
#define DMA_HFILE

#include <stdint.h>

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
