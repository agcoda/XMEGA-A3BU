/*
 * main.c
 *
 * Created: 10/10/2023 8:17:17 PM
 *  Author: Alex Gauf
 */ 

/* Assignment
1a. Create an include file which contains hardware specific information
	
1b. Create wrapper functions to encapsulate Register Read/Write operations
	
2.Turn ON and OFF the LEDs continuously with the ON and OFF duration
to be approximately between 1 and 2 seconds.
3.Use a software loop to create a delay routine to control the ON and OFF
duration
4.DO NOT use the delay library functions provided in the AVR libraries.
	
*/


#include <devreg.h>

//protos
void WriteReg(volatile unsigned char *, unsigned char );
volatile unsigned char ReadReg(volatile unsigned char *);
void Delay(int);
void HmSDelay(void);

int main (void)

{
	static unsigned char ledStates[2] = {0xff, 0xfc}; //{all off, all on}
	unsigned char stateSwitch = 1;
		
	//set PORTB DATA direction (DDRB) as output
	WriteReg((volatile unsigned char *)PORTB_DATA,0xff);
			
	while(1){
		//Alternate writing ..00 and ..11 to the LED register
		WriteReg((volatile unsigned char *)PORTB_LED_STATE,ledStates[stateSwitch]);
		
		//delay 15*100ms should give ~1.5s per on/off cycle
		Delay(15);
		
		stateSwitch = (stateSwitch+1)%2; 
	}
}

//write Value to RegAddress 
void WriteReg(volatile unsigned char * p_regAddress, unsigned char Value){
	*(p_regAddress) = Value;
}

//read value in Regaddress and return 
volatile unsigned char ReadReg(volatile unsigned char * p_regAddress){
	return *(p_regAddress);
}

//delay an integer number of 100ms (ds)
void Delay(int dS){
	for(int i =0; i<dS;i++){
		HmSDelay();
	}
}

//Use register read to delay approximately 100ms.
void HmSDelay(){
	for(int i=0;i<H_MS_DELAY_CT;i++){
		ReadReg((volatile unsigned char *)PORTB_LED_STATE);
	}
			
}

