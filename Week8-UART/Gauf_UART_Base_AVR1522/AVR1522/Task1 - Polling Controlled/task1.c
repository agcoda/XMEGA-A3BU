/*******************************************************************************
 *           UART Echo
			Alex Gauf 11/19
 *	Code will wait for a received char on the Serial input RX of J1
 *	Once received it will echo the character back
 ******************************************************************************/

#include <avr/io.h>
#include <stdbool.h>

//use this for simple delays and led blinking for debug purposes.
//uncomment "debugBlink" lines as needed
//#include "ledSwitchDebug.h"

/* The board.h header file defines which IO ports peripherals like
 * switches and LEDs are connected to.
 */
#include "../board.h"

#define BSCALE_VALUE  0
#define BSEL_VALUE   12

#define TEST_CHARS  240 //smy 100



void debugBlink(unsigned);

int main(void)
{

	volatile char Tx_Buf[TEST_CHARS];
	volatile char Rx_Buf[TEST_CHARS];

	//watching for transfer errors not in spec
	//bool TransferError = false;

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
	//debugBlink(3);
	
	//store data messages as predefined strings
	#define WAITMSG_SIZE	20
	#define RECMSG_SIZE		10
	#define ENDMSG_SIZE		4
	char waitingMsg[WAITMSG_SIZE] = "|Enter any char: ";
	char receivedMsg[RECMSG_SIZE] = "Received: ";
	char endMsg[ENDMSG_SIZE] = "   |";
	
	
	while(1){
		//let them know we're waiting
		for(int i =0; i<WAITMSG_SIZE;i++){
			USART.DATA = waitingMsg[i];
			while ((USART.STATUS & USART_DREIF_bm) == 0){}
		}
		//Wait for serial receive on RX
		while ((USART.STATUS & USART_RXCIF_bm) == 0){}
		//once received store in rxbuf
		Rx_Buf[0] = USART.DATA;
		
		//msg to notify we're echoing
		for(int i =0; i<RECMSG_SIZE;i++){
			USART.DATA = receivedMsg[i];
			while ((USART.STATUS & USART_DREIF_bm) == 0){}
		}
	
		//send back char recd
		Tx_Buf[0] = Rx_Buf[0];
		USART.DATA = Tx_Buf[0];
		while ((USART.STATUS & USART_DREIF_bm) == 0){}
			
		//wrap up the line nicely
		//probably replace with newline but Tera Term wasn't reading \n
		for(int i =0; i<ENDMSG_SIZE;i++){
			USART.DATA = endMsg[i];
			while ((USART.STATUS & USART_DREIF_bm) == 0){}
		}
//		debugBlink(2);
	}
	
//Task 1 example code below here
	/*
for( int x=0; x < 10; x++)
{
	i = 0;
	while (i != TEST_CHARS){
		
		Tx_Buf[i] = i;
		// Wait for previous character to be sent 
		while ((USART.STATUS & USART_DREIF_bm) == 0){}
		// Transmit current character 
		USART.DATA = Tx_Buf[i];   
	
		// Wait until data is received 
		//while ((USART.STATUS & USART_RXCIF_bm) == 0){}
		// Read the received data 
		Rx_Buf[i] = USART.DATA;

		// Compare buffers 
		if (Rx_Buf[i] != Tx_Buf[i]){
			TransferError = true;
		}
		i++;
		debugBlink(1);
	}
		} //smy for
		debugBlink(5);
	while (1){
		// Check for transfer error 
		if (TransferError){
			// No light 
			//LEDPORT.OUT = 0xFF;
			debugBlink(1);
		} else {  
            // Light          
            //LEDPORT.OUT = 0x00;
			debugBlink(2);
		}
	}
	debugBlink(2);
	*/
}

