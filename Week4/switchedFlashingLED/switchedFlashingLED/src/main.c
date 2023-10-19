/*
 * main.c
 *
 * Created: 10/10/2023 8:17:17 PM
 *  Author: Alex Gauf
 */ 

/* Assignment
1. Create a code using Robin Robin architecture to blink both LED’s when no switch
is pressed just like previous assignment #1.

2. When any switch is pressed, then only blink the LED(s) associated to the switch
number. Use the SW1 and SW2 switches.
See the link provided to get information on the port being used for these switches
on the XMega Xplained board. Note the port being used for the Switches will be
configured as an input port.
http://ww1.microchip.com/downloads/en/AppNotes/doc8394.pdf

3. Create a function that controls the blink operation

4. The main task loop should check the switch setting, and make decision
whether to blink both LED’s one at a time OR only blink the LEDS associated to the
switches that are pressed.
	
*/


#include <devreg.h>

//protos
void blink(unsigned *, unsigned *,unsigned);
unsigned swStateCheck(unsigned);
void toggleLED(unsigned);
void WriteReg(unsigned, unsigned char );
unsigned ReadReg(unsigned);
void HmSDelay(void);
void Delay(unsigned int);

//debug protos
//void debugBlink(unsigned );


int main (void)
{
						
	while(1){
		unsigned sw1 = 0;
		unsigned sw2 = 0;
		//round robin polling
		//check switch 1
		sw1 = swStateCheck(1);
		
		//check switch 2
		sw2 = swStateCheck(2);
		
		blink(&sw1,&sw2,15);
		
	
	}
}

//blink leds 1,0 based on 1/0 in referenced for delay hundred ms delay
void blink(unsigned *p_sw1, unsigned *p_sw2, unsigned delay){
		
		//if either or both button pressed blink on off before checking states again
		//if neither pressed still blink both
		for(int i=0;i<2;i++){
			if(*p_sw1==1 || *p_sw2==1){
				if(*p_sw1==1){
					toggleLED(LED0);
				}
				if(*p_sw2==1){
					toggleLED(LED1);
				}
			}
			else{
				toggleLED(LED0);
				toggleLED(LED1);
			}
			Delay(delay);
		}
}

//check the bit associated with one of the three switches
//generalized for three switches for now
unsigned swStateCheck(unsigned swNum){
	unsigned int swState = 0x00;
	
	//switch 0 is on port E bit 5
	//switch 1 and 2 are on port F bit 1 and 2
	unsigned portData[3]={PORTE_DATA, PORTF_DATA, PORTF_DATA}; 
	unsigned portIn[3]={PORTE_INPUT, PORTF_INPUT, PORTF_INPUT};
	unsigned swMask[3] = {BIT5_MASK, BIT1_MASK,BIT2_MASK};
	
	//first call needs to init proper ports as input then init the output array
	static unsigned portInit[3] = {1,1,1}; //{portF, portE}
	
	//writing entire dir register as input for efficiency
	//need to switch to indiv bits if we use more of the port
	if(portInit[swNum] == 1){
		WriteReg(portData[swNum],0x00);
		portInit[swNum] =0;
	}
	
	//When a button is pressed it will drive the I/O line to GND
	//so the needed bit will be low
	swState = ~ReadReg(portIn[swNum]);
	
	//debugBlink(swState); 
	
	if((swState &= swMask[swNum]) == swMask[swNum]){
		return 1;
	}
	else{
		return 0;
	}
	
}

//ledx toggle
void toggleLED(unsigned ledNum){
	//first call needs to set portR as output then init the output array
	static unsigned init = 1;
	if(init == 1){
		WriteReg(PORTR_DATA,0xff);
		WriteReg(PORTR_OUTPUT,0xff);
		init =0;
	}
	
	//writing 1 to the appropriate ledByte TOGGLES the LED
	switch(ledNum){
	case LED0:
		WriteReg(PORTR_OUTPUT, BIT0_MASK);
		break;
	case LED1:		
		WriteReg(PORTR_OUTPUT, BIT1_MASK);
		break;
	}
	
}

//write Value to RegAddress 
void WriteReg(unsigned regAddress, unsigned char Value){
	*((volatile unsigned char *)regAddress) = Value;
}

//read value in Regaddress and return 
unsigned ReadReg(unsigned regAddress){
	return *((volatile unsigned char *)regAddress);
}

//should delay about 100ms
void HmSDelay(){
	//utilizing delay provided from prev sols
	volatile unsigned int temp;
	for(temp = 0; temp < H_MS_DELAY_CT; temp++);
}
//delay an integer number of 100ms (ds)
void Delay(unsigned int val)
{
	for(unsigned int i=0;i<val;i++){
		HmSDelay();
	}
}
//blink led if line reached
//for debug only
/*
void debugBlink(unsigned times){
	for(int i=0;i<times;i++){
		blink(0,0,1);
	}
Delay(3);
}
*/