/*******************************************************************************
 *                      Task 3: USART using DMAC
 *
 * This task will show how to set up the USART with the DMA controller. 
 * SetupTransmitChannel and SetupReceiveChannel are using the dma_driver.c
 * The driver provides a fast setup of the DMAC. SetupTransmitChannel 
 * is setting up the Tx_Buf buffer to feed the USART data transfer register. 
 * At the receiver, another DMAC channel will receive the bytes and 
 * start filling the Rx_Buf buffer.
 ******************************************************************************/

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "../dma_driver.h"

/* The board.h header file defines which IO ports peripherals like
 * Switches and LEDs are connected to
 */
#include "../board.h"

/* 2MHz internal RC oscillator, 9600Baud: */
#define BSEL_VALUE 12
#define BSCALE_VALUE 0

#define TEST_CHARS 20

#define DMA_TX_Channel &DMA.CH0
#define DMA_RX_Channel &DMA.CH1

/* Buffers for storing transmitted and received data */
static char Tx_Buf[TEST_CHARS];
static char Rx_Buf[TEST_CHARS];

void SetupTransmitChannel(void)
{
	DMA_SetupBlock( 
		DMA_TX_Channel,
		Tx_Buf,
		DMA_CH_SRCRELOAD_NONE_gc, 
		DMA_CH_SRCDIR_INC_gc,
		(void *)&USART.DATA,
		DMA_CH_DESTRELOAD_NONE_gc, 
		DMA_CH_DESTDIR_FIXED_gc,
		TEST_CHARS, 
		DMA_CH_BURSTLEN_1BYTE_gc, 
		0, /* Perform one transfer only */
		false);
	DMA_EnableSingleShot(DMA_TX_Channel);
	/* USART Trigger source, Data Register Empty flag */
	DMA_SetTriggerSource( DMA_TX_Channel, DMA_CH_TRIGSRC_USART_DRE_gc); 
}

void SetupReceiveChannel(void)
{
	DMA_SetupBlock( 
		DMA_RX_Channel,
		(void *)&USART.DATA,
		DMA_CH_SRCRELOAD_NONE_gc, 
		DMA_CH_SRCDIR_FIXED_gc,
		Rx_Buf,
		DMA_CH_DESTRELOAD_NONE_gc, 
		DMA_CH_DESTDIR_INC_gc,
		TEST_CHARS, 
		DMA_CH_BURSTLEN_1BYTE_gc, 
		0, /* Perform one transfer only */
		false);
        
    DMA_EnableSingleShot(DMA_RX_Channel);
    /* USART Trigger source, Receive Complete flag */
    DMA_SetTriggerSource(DMA_RX_Channel, DMA_CH_TRIGSRC_USART_RXC_gc); 
}

void SetupUsart(void)
{
    /* Place a jumper to connect Pin 3 and Pin 2 on J1 */
    
	/* (TX0) as output */
	USART_PORT.DIRSET = PIN3_bm;
	/* (RX0) as input */
	USART_PORT.DIRCLR = PIN2_bm; 
	/* USARTD0; 8 Data bits, No Parity, 1 Stop bit */
	USART.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc 
		| USART_PMODE_DISABLED_gc 
		| false;
    /* Enable both TX and RX port operation */
    USART.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
 
    /* Target: Internal RC 2MHz (default), 9600baud  */
    USART.BAUDCTRLA = BSEL_VALUE;
}

int main(void)
{
	uint16_t i = 0;    
	bool success;

	SetupUsart();

    /* Initialize LEDs */
    LEDPORT.DIR = 0xFF;
    LEDPORT.OUT = 0xFF;
    
    /* Initialize DMAC */
    DMA_Enable();
    SetupTransmitChannel();
    SetupReceiveChannel();
    
    /* Initialize transmit buffer */
    for(i=0; i < TEST_CHARS; i++)
    {
        /* Filling with letters a, b, c ... t */
        Tx_Buf[i] = 'a'+ i;
    }
    
    /* Assume that everything is OK */
    success = true;
    
	/* Initialize the receiving DMA channel to wait for characters 
	 * and write them to Rx_Buf
	 */
    DMA_EnableChannel(DMA_RX_Channel);

	/* Start sending the data bits from Tx_Buf on the
	 * transmit DMA channel.
	 * Note that DRE is considered empty at this point,	
	 * to trigger the initial transfer
	 */
    DMA_EnableChannel(DMA_TX_Channel);
    
    /* While DMA is handling the USART, we can do something else with the CPU.
	 * In this case something as simple is counting upwards
	 */
    i = 0;
	while (!DMA_ReturnStatus_non_blocking(DMA_RX_Channel)){
		i++;
	}

	/* DMA transfer has completed
	 * Show the lower bits of the counter on the LEDs
	 */
	LEDPORT.OUT = i & 0xFF;

	/* Verify the received characters */
    for (i = 0; i < TEST_CHARS; i++){
        if (Tx_Buf[i] != Rx_Buf[i]){
            /* Something went wrong... */
            success = false;
        }
    }
    
    while (1){
        if (success){
            if (i > TEST_CHARS){
                i = 0;
            } else {
                i++;
            }
			/* Note that the operation will be repeated and
			 * that the result will be rewritten.
			 * Output the result to the LEDs  
			 */     
			_delay_ms(200);
			LEDPORT.OUT = Rx_Buf[i];
        } else {
			/* No lights means failure! */
			LEDPORT.OUT = 0xFF;
        }
    }
}
