/*******************************************************************************
 *           
 *                           -= XMEGA USART Task 1 =-
 *
 *        Task 1: USART in polling mode
 *
 *        The application sends and receives TEST_CHARS bytes, in a loop-
 *        back connection. Latch the TX(PD3) to RX(PD2) and start having fun.
 *
 *                                 -= XMEGA =-
 *
 ******************************************************************************/

#include <avr/io.h>
#include <stdbool.h>

/* The board.h header file defines which IO ports peripherals like
 * switches and LEDs are connected to.
 */
#include "../board.h"

#define BSCALE_VALUE  0
#define BSEL_VALUE   12

#define TEST_CHARS  240 //smy 100


/*  Example USART application.
 *
 *  This example configures USARTx0 with the parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 *
 *  This function will send the values 0-100 and test if the received data is
 *  equal to the sent data. The code can be tested by connecting PF3 to PF2 
 *  which is RXD and TXD on the header J1 on the XMEGA-A1 Xplained board. If
 *  the variable 'TransferError' is false and LEDs are lit up at the end of
 *  the function, the bytes have been successfully sent and received by USARTx0.
 */

void debugBlink(unsigned);

int main(void)
{
  
	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;
 
 	uint8_t i;

	volatile char Tx_Buf[TEST_CHARS];
	volatile char Rx_Buf[TEST_CHARS];

	bool TransferError = false;

	/* Pin 3 (TX) as output */
	USART_PORT.DIRSET   = PIN3_bm;   
	/* Pin 2 (RX) as input */
	USART_PORT.DIRCLR   = PIN2_bm;

	/* USARTx0, 8 Data bits, No Parity, 1 Stop bit */
	USART.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc 
		| USART_PMODE_DISABLED_gc 
		| false;

	/* Target: Internal RC 2MHz (default) 
	 * Example (9600bps)  :   - I/O clock = 2MHz
	 *                        - 9600bps can be achieved by 9600bps / 2^0
	 *                        - UBRR = 2MHz/(16*9600)-1 = 12.02
	 *                        - ScaleFactor = 0
	 */
	USART.BAUDCTRLA = BSEL_VALUE;

	/* Enable both RX and TX */
	USART.CTRLB |= USART_RXEN_bm;
	USART.CTRLB |= USART_TXEN_bm;
	debugBlink(3);
for( int x=0; x < 10; x++)
{
	i = 0;
	while (i != TEST_CHARS){
		
		Tx_Buf[i] = i;
		/* Wait for previous character to be sent */
		while ((USART.STATUS & USART_DREIF_bm) == 0){}
		/* Transmit current character */
		USART.DATA = Tx_Buf[i];   
	
		/* Wait until data is received */
		while ((USART.STATUS & USART_RXCIF_bm) == 0){}
		/* Read the received data */
		Rx_Buf[i] = USART.DATA;

		/* Compare buffers */
		if (Rx_Buf[i] != Tx_Buf[i]){
			TransferError = true;
		}
		i++;
	}
		} //smy for
	while (1){
		/* Check for transfer error */
		if (TransferError){
			/* No light */
			LEDPORT.OUT = 0xFF;
		} else {  
            /* Light */          
            LEDPORT.OUT = 0x00;
		}
	}
}


#define LED0 0
#define LED1 1

#define PORTR_DATA			0x07E0		//PORT B Data Direction
#define PORTR_OUTPUT		0x07E7		//Address for setting LED1 and LED0
#define PORT_OUTPUT			0x7
#define BIT0_MASK 0x01
#define BIT1_MASK 0x02
#define H_MS_DELAY_CT 11160			//each read is about 9 us from manual timing so 11160 reads is 100 ms.

//write Value to RegAddress
void WriteReg(unsigned regAddress, unsigned char Value){
	*((volatile unsigned char *)regAddress) = Value;
}

//read value in Regaddress and return
unsigned ReadReg(unsigned regAddress){
	return *((volatile unsigned char *)regAddress);
}
//ledx toggle
void toggleLED(unsigned ledNum){
	//first call needs to set portR as output then init the output array
	static unsigned init = 1;
	if(init == 1){
		WriteReg(PORTR_DATA,0xff);
		WriteReg(PORTR_DATA | PORT_OUTPUT,0xff);
		init =0;
	}
	
	//writing 1 to the appropriate ledByte TOGGLES the LED
	switch(ledNum){
		case LED0:
		WriteReg(PORTR_DATA | PORT_OUTPUT, BIT0_MASK);
		break;
		case LED1:
		WriteReg(PORTR_DATA | PORT_OUTPUT, BIT1_MASK);
		break;
	}
}
//blink led if line reached
//for debug only
//delay an integer number of 100ms (ds)
void HmSDelay(){
	for(int i=0;i<H_MS_DELAY_CT;i++){
		ReadReg(PORTR_OUTPUT);
	}
	
}
void Delay(int dS){
	for(int i =0; i<dS;i++){
		HmSDelay();
	}
}

//Use register read to delay approximately 100ms.

void debugBlink(unsigned times){
	for(int i=0;i<times;i++){
		for(int i=0;i<2;i++){

			toggleLED(LED0);
			toggleLED(LED1);
			Delay(1);
		}
	}
	Delay(5); 
}
