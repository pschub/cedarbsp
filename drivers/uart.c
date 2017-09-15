/* uart.c
 * Interface to UART0 on teensy3, using pins 0 and 1. ISR driven, with ring
 * buffer backing TX and RX.
 * Written with the help of shamelessly consulting/stealing from
 * Teensyduino/PJRC serial1.c code.
 * Patrick Schubert 2017
 */
#include "kinetis.h"
#include "uart.h"
#include "circbuff.h"

// Size of RX and TX ring buffers
#define TX_BUF_SIZE (64)
#define RX_BUF_SIZE (64)

#define FIFO_DEPTH (8)

CircularBuffer rx_buf;
CircularBuffer tx_buf;

uint8_t rxb[RX_BUF_SIZE];
uint8_t txb[TX_BUF_SIZE];

// Bitfield to keep track of things that break. Read with uart_geterror.
static volatile uint8_t uart_errors = 0;

/* Initialize UART
 *
 * 4.7us
 */
void uart_init(uint32_t baud)
{
    cb_init(&rx_buf, rxb, RX_BUF_SIZE);
    cb_init(&tx_buf, txb, TX_BUF_SIZE);

    uart_errors = 0;

    // enable clock
    SIM_SCGC4 |= SIM_SCGC4_UART0;

    // enable pins as UART, with pullups.
    PORTB_PCR16 = PORT_PCR_MUX(0x3) | PORT_PCR_PE | PORT_PCR_PS;
    PORTB_PCR17 = PORT_PCR_MUX(0x3) | PORT_PCR_PE | PORT_PCR_PS;

    // disable uart while settings change.
    UART0_C2 = 0;

    // clear settings
    UART0_C1 = 0;
    UART0_C3 = 0;
    UART0_C4 = 0;
    UART0_C5 = 0;

    // Clear any preexisting data
    UART0_CFIFO |= UART_CFIFO_TXFLUSH | UART_CFIFO_RXFLUSH;

    // Enabled FIFOs
    UART0_PFIFO |= UART_PFIFO_TXFE | UART_PFIFO_RXFE;
    UART0_TWFIFO = 2;
    UART0_RWFIFO = 4;

    // Baud generator math from Teensyduino
    #define BAUD2DIV(baud) (((F_CPU * 2) + ((baud) >> 1)) / (baud))
    uint32_t div = BAUD2DIV(baud);
    UART0_BDH = (div >> 13) & 0x1F;
    UART0_BDL = (div >> 5) & 0xFF;
    UART0_C4 = div & 0x1F;

    UART0_C1 |= UART_C1_ILT; // Idle count starts after STOP bit.
    UART0_C2 |= UART_C2_RIE;
    UART0_C2 |= UART_C2_ILIE;

    UART0_C2 |= UART_C2_TE | UART_C2_RE; // Enable UART TX/RX

    NVIC_ENABLE_IRQ(IRQ_UART0_STATUS);
    NVIC_SET_PRIORITY(IRQ_UART0_STATUS, 64);


    return;
}


int uart_putc(uint8_t c)
{
    int ret = cb_putc(&tx_buf, c);
    if (ret){
        // Buff full! Enable transmit int just in case.
        UART0_C2 |= UART_C2_TIE;
        return -1;
    }

    UART0_C2 |= UART_C2_TIE;
    return 0;
}


int uart_puts(uint8_t* s)
{
    if (!s) return -1;
    while (*s != '\0'){
        int ret = uart_putc(*s);
        if (ret){
            return ret;
        }
        s++;
    }
    return 0;
}

int uart_puts_wait(uint8_t* s)
{
    if (!s) return -1;
    while(*s != '\0') {
        if (0 == uart_putc(*s)){
            s++;
        }
    }
    return 0;
}


int uart_getc(uint8_t* c)
{
    if (cb_isempty(&rx_buf)){
        return -1;
    }
    *c = cb_getc(&rx_buf);
    return 0;
}


void uart_setparity(uint8_t type)
{
    switch(type){
        case (UART_EVEN_PARITY):
            UART0_C1 &= ~(UART_C1_PT);
            UART0_C1 |= UART_C1_PE | UART_C1_M;
            break;
        case (UART_ODD_PARITY):
            UART0_C1 |= UART_C1_PE | UART_C1_PT | UART_C1_M;
            break;
        default:
            UART0_C1 &= ~(UART_C1_PE);
    }
    return;
}


uint32_t uart_geterror(void)
{
    uint32_t e = (uint32_t) uart_errors;
    uart_errors = 0;
    return e;
}


// Tested: ISR takes around 11us. At 115200 constant transmission, tx empties
// every 575 us
void uart0_status_isr(void)
{
    uint8_t avail, c;
    uint8_t status = UART0_S1;

    /*  RECEIVER INTERRUPT */

    if (status & (UART_S1_RDRF | UART_S1_IDLE)){
        __disable_irq();
        avail = UART0_RCFIFO;
        if (avail == 0){
            // IDLE-handling code from Teensyduino (serial1.c). Freescale
            // engineers making life difficult.
            // To clear IDLE, need to read DATA, but if nothing's available, then
            // that will cause an underrun, so we clear that with a flush, but
            // hopefully we don't receive a character during this in-between time.
            c = UART0_D;
            UART0_CFIFO = UART_CFIFO_RXFLUSH;
            __enable_irq();
        } else {
            __enable_irq();
            if (status & UART_S1_PF){
                // Parity Error
                c = UART0_D;
                UART0_CFIFO = UART_CFIFO_RXFLUSH;
                uart_errors |= UART_ERROR_PARITY;
            }
            while (UART0_RCFIFO){
                c = UART0_D;
                if (cb_putc(&rx_buf, c)){
                    // Ring buffer is full
                    uart_errors |= UART_ERROR_RXOVER;
                }
            }
        }
    }

    /* TRANSMITTER INTERRUPT */
    if (UART0_C2 & UART_C2_TIE) {
        if (cb_isempty(&tx_buf)){
            // If we emptied the buffer, then turn off fifo interrupt.
            UART0_C2 &= ~(UART_C2_TIE);
        } else if (UART0_S1 & UART_S1_TDRE){
            // fifo is empty and cb has data, so fill the fifo.
            do {
                status = UART0_S1;
                UART0_D = cb_getc(&tx_buf);
                if (cb_isempty(&tx_buf)){
                    break;
                }
            } while (UART0_TCFIFO < FIFO_DEPTH);
        }
    }
}


