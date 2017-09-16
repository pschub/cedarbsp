
#include "kinetis.h"
#include "dma.h"

#define DMAMUX_SRC_MASK (0x3F)
#define DMAMUX_CHCFG(n)    (*(volatile uint8_t*)(0x40021000 + n))

// Macros to get DMA regs based off channel#.
#define DMA_TCD_OFFSET (0x20)
#define DMA_TCD_SADDR(n)   (*(volatile uint32_t**)(0x40009000 + n*DMA_TCD_OFFSET))
#define DMA_TCD_SOFF(n)    (* (volatile int16_t*)(0x40009004 + n*DMA_TCD_OFFSET))
#define DMA_TCD_ATTR(n)    (*(volatile uint16_t*)(0x40009006 + n*DMA_TCD_OFFSET))
#define DMA_TCD_NBYTES(n)  (*(volatile uint32_t*)(0x40009008 + n*DMA_TCD_OFFSET))
#define DMA_TCD_SLAST(n)   (* (volatile int32_t*)(0x4000900C + n*DMA_TCD_OFFSET))
#define DMA_TCD_DADDR(n)   (*(volatile uint32_t**)(0x40009010 + n*DMA_TCD_OFFSET))
#define DMA_TCD_DOFF(n)    (* (volatile int16_t*)(0x40009014 + n*DMA_TCD_OFFSET))
#define DMA_TCD_CITER(n)   (*(volatile uint16_t*)(0x40009016 + n*DMA_TCD_OFFSET))
#define DMA_TCD_DLAST(n)   (* (volatile int32_t*)(0x40009018 + n*DMA_TCD_OFFSET))
#define DMA_TCD_CSR(n)     (*(volatile uint16_t*)(0x4000901C + n*DMA_TCD_OFFSET))
#define DMA_TCD_BITER(n)   (*(volatile uint16_t*)(0x4000901E + n*DMA_TCD_OFFSET))


void dma_init(void)
{
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
    SIM_SCGC7 |= SIM_SCGC7_DMA;

    DMA_CR = 0;
    return;
}

void dma_set_mux(uint32_t ch, uint32_t source)
{
    if (DMA_NUM_CHANNELS <= ch){
        // out of range
        return;
    }

    DMAMUX_CHCFG(ch) = DMAMUX_DISABLE;
    DMAMUX_CHCFG(ch) = DMAMUX_ENABLE | (DMAMUX_SRC_MASK & source);
    return;
}

void dma_configure(uint32_t ch, DMA_TCD* tcd)
{
    if (DMA_NUM_CHANNELS <= ch){
        // out of range
        return;
    }

    DMA_TCD_SADDR(ch) = tcd->source;
    DMA_TCD_SOFF(ch) = tcd->soff;

    DMA_TCD_DADDR(ch) = tcd->dest;
    DMA_TCD_DOFF(ch) = tcd->doff;

    tcd->size = tcd->size & 0x7;
    DMA_TCD_ATTR(ch) =  DMA_TCD_ATTR_SSIZE(tcd->size)
                      | DMA_TCD_ATTR_DSIZE(tcd->size)
                      | DMA_TCD_ATTR_DMOD(tcd->dmod)
                      | DMA_TCD_ATTR_SMOD(tcd->smod);

    DMA_TCD_NBYTES(ch) = tcd->nbytes;
    DMA_TCD_BITER(ch) = (0x7fffffffffffffff & tcd->citer);
    DMA_TCD_CITER(ch) = (0x7fffffffffffffff & tcd->citer);

    DMA_TCD_SLAST(ch) = tcd->slast;
    DMA_TCD_DLAST(ch) = tcd->dlast;

    DMA_TCD_CSR(ch) = 0;
}

void dma_enable(uint32_t ch)
{
    DMA_SERQ = DMA_SERQ_SERQ(ch);
}

void dma_enable_int(uint32_t ch)
{
    if (DMA_NUM_CHANNELS <= ch){
        // out of range
        return;
    }

    DMA_TCD_CSR(ch) |= DMA_TCD_CSR_INTMAJOR;
    NVIC_ENABLE_IRQ(ch);
    return;
}

