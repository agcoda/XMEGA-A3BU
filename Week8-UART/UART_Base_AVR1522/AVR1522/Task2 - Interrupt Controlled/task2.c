/*******************************************************************************
 *           
 *                           -= XMEGA =-
 *
 *        Task 2: USART Using driver and ring buffer in interrupt mode
 *
 *        The application sends NUM_BYTES bytes, then waits for NUM_BYTES 
 *        bytes to be received. The transmission is done using one USART, 
 *        USARTD0 (TX/PF3 and RX/PF2). The code can be tested by 
 *        connecting PF2 to PF3. If the variable 'TransferError' is false  
 *        at the end of the file, the NUM_BYTES bytes have been successfully 
 *        transmitted and received. The task will also give insight in the 
 *        ring buffer that is implemented in the driver.
 *
 *                           -= XMEGA =-
 *
 ******************************************************************************/

#include "../usart_driver.h"
#include "../avr_compiler.h"

/* The board.h header file defines which IO ports peripherals like
 * Switches and LEDs are connected to 
 */
#include "../board.h"

/* Number of bytes to send in this task */
#define NUM_BYTES  3

/* LED port definition */
#define LEDPORT PORTE

/* USART data struct used in task */
USART_data_t USART_data;

/* Array containing test data to send */
volatile uint8_t sendArray[NUM_BYTES] = {0x55, 0xaa, 0xf0};

/* Array to store received data */
volatile uint8_t receiveArray[NUM_BYTES];

/* Variable to save errors */
volatile bool TransferError = false;


/*  This code in this task configures USARTD0 with these parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 */
int main(void)
{
	/* Counter variable */
	uint8_t i;

	/* LEDs as output */
	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;

	/* (TX) as output */
	USART_PORT.DIRSET = PIN3_bm;
	/* (RX) as input */
	USART_PORT.DIRCLR = PIN2_bm;
    
	/* Use USARTx0 and initialize buffers */
	USART_InterruptDriver_Initialize(&USART_data, 
		&USART, 
		USART_DREINTLVL_LO_gc);
    
	/* USARTx0, 8 Data bits, No Parity, 1 Stop bit */
	USART_Format_Set(USART_data.usart, 
		USART_CHSIZE_8BIT_gc,
		USART_PMODE_DISABLED_gc, 
		false);
    
	/* Enable RXC interrupt */
	USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_LO_gc);
    
	/* Set baud rate to 9600 bps:
	 * Use the default I/O clock frequency that is 2 MHz.
	 * Do not use the baudrate scale factor
	 *
	 * Baud rate select	= (1 / (16 * (((I/O clock frequency) / Baudrate) - 1)
	 *					= 12
	 */

	USART_Baudrate_Set(&USART, 12 , 0);

	/* Enable both RX and TX. */
	USART_Rx_Enable(USART_data.usart);
	USART_Tx_Enable(USART_data.usart);

	/* Enable PMIC interrupt level low */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts */
	sei();

	/* Send array */
	i = 0;
	while (i < NUM_BYTES){
		bool byteToBuffer;
		
		byteToBuffer = USART_TXBuffer_PutByte(&USART_data, sendArray[i]);		
		if (byteToBuffer){
			i++;
		}
	}

	/* Fetch received data as it is received */
	i = 0;
	while (i < NUM_BYTES){
		if (USART_RXBufferData_Available(&USART_data)){
			receiveArray[i] = USART_RXBuffer_GetByte(&USART_data);
			i++;
		}
	}

	/* Test to see if sent data equals received data */
	/* Assume no Errors first */
	TransferError = false;
	for (i = 0; i < NUM_BYTES; i++){
		/* Check that each element is received correctly */
		if (receiveArray[i] != sendArray[i]){
			TransferError = true;
		}
	}

	/* Show status of the transfer */
	while (1){
		if (TransferError){
			/* No light */
			LEDPORT.OUT = 0xFF;
		} else {  
			/* Light */    
			LEDPORT.OUT = 0x00;
		}
	}
}


/*  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 *
 *  Note that the vector name is a hash define mapped to the correct interrupt
 *  vector. See "board.h".
 */
ISR(USART_RXC_vect)
{                     
    USART_RXComplete(&USART_data);
}


/*  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 *
 *  Note that the vector name is a hash define mapped to the correct interrupt
 *  vector. See "board.h".
 */
ISR(USART_DRE_vect) 
{
    USART_DataRegEmpty(&USART_data);
}

