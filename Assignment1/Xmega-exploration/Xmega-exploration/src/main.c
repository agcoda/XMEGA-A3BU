/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
//#include <asf.h>
#include <devreg.h>

//protos
void WriteReg(volatile unsigned char *, unsigned char );
volatile unsigned char * ReadReg(volatile unsigned char *);
void delay();

int main (void)
{
	/*
	2.Turn ON and OFF the LEDs continuously with the ON and OFF duration
	to be approximately between 1 and 2 seconds.
	3.Use a software loop to create a delay routine to control the ON and OFF
	duration
	4.DO NOT use the delay library functions provided in the AVR libraries.
	
	Create an include file which contains hardware specific information
	? Example:
	Create a file called devreg.h
	Include Memory mapped addresses related to the PORT and
	GPIO’s addresses being used
	You will keep adding other PORT and GPIO’s addresses on to this file during
	the course
	? Create wrapper functions to encapsulate Register Read/Write operations
	? Example:
	Create Function WriteReg(RegAddress, Value)
	Create Function ReadReg(RegAddress)
	*/
	int ledOn = 1;
		
	//set PORTB DATA direction (DDRB) as output
	//*((volatile unsigned char *)PORTB_DATA) = 0xff;
	WriteReg(PORTB_DATA,0xff);
	
	//Turn LED0 ON and LED 1 OFF by writing 0xFE to output PORTB,
	//bit 0 set to 0 to turn on LED 0
	//*((volatile unsigned char *)PORTB_LED_STATE) = 0xfe;
	
	
	while(1){
		volatile unsigned char value = 0;
		
		WriteReg((volatile unsigned char *)PORTB_LED_STATE,0xfe);
		
		//currently works with delay up to maxInt (32k) but doesn't flash over that
		for(int i =0; i<10;i++){
			delay(30000);
		}
	


		WriteReg((volatile unsigned char *)PORTB_LED_STATE,0xff);
		
		/*
		//refactor
		unsigned char onOff = 0xfe;
		onOff |= ledOn;
				
		WriteReg(PORTB_LED_STATE,onOff);
		for(int i=0;i<delay;i++){
			ReadReg(PORTB_LED_STATE);
		}
		
		//flip each pass if we're switching on or off
		ledOn = (ledOn+1)%2;
		*/
	}

	/* Insert application code here, after the board has been initialized. */


}

void WriteReg(volatile unsigned char * RegAddress, unsigned char Value){
	
	*(RegAddress) = Value;
}

volatile unsigned char * ReadReg(volatile unsigned char * RegAddress){
	return *(RegAddress);
}


void delay(int loops){
		for(int i=0;i<loops;i++){
			ReadReg(PORTB_LED_STATE);
		}
			
}