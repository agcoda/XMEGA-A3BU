
/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

#include <devreg.h>

//protos
void WriteReg(volatile unsigned char *, unsigned char );
volatile unsigned char * ReadReg(volatile unsigned char *);
void delay();

int main (void)
{
	static unsigned char ledStates[2] = {0xff, 0xfc}; //{all off, all on}
	unsigned char stateSwitch = 0;
	/*
	1a. Create an include file which contains hardware specific information
	
	1b. Create wrapper functions to encapsulate Register Read/Write operations
	
	2.Turn ON and OFF the LEDs continuously with the ON and OFF duration
	to be approximately between 1 and 2 seconds.
	3.Use a software loop to create a delay routine to control the ON and OFF
	duration
	4.DO NOT use the delay library functions provided in the AVR libraries.
	
	*/
		
	//set PORTB DATA direction (DDRB) as output
	WriteReg(PORTB_DATA,0xff);
	
	
		
	while(1){
		
		//Turn LED0 ON and LED 1 OFF by writing 0xFE to output PORTB,
		//bit 0 set to 0 to turn on LED 0
		WriteReg((volatile unsigned char *)PORTB_LED_STATE,ledStates[stateSwitch]);
		
		//delay 15*100ms should give ~1.5s per on/off cycle
		for(int i =0; i<15;i++){
			delay();
		}
		stateSwitch = (stateSwitch+1)%2; 
	}
}

//write Value to RegAddress 
void WriteReg(volatile unsigned char * RegAddress, unsigned char Value){
	*(RegAddress) = Value;
}

//read value in Regaddress and return 
volatile unsigned char * ReadReg(volatile unsigned char * RegAddress){
	return *(RegAddress);
}

//Read the state of the LEDs to force a delay
//Hard coded to delay 100ms
void delay(){
		for(int i=0;i<H_MS_DELAY_CT;i++){
			ReadReg(PORTB_LED_STATE);
		}
			
}