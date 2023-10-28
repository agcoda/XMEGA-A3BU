/*
 * main.c
 *
 * Created: 10/24/2023 
 *  Author: Alex Gauf
 */ 

/* Assignment 4
1. Use the Xmega timer 0(TCC0) hardware to create delay code by replacing
the software delay loop used in Programming assignment #3. Look at section 34 
for the base address for the TCC0 timer and section 14 for the details 
on the usage timer in the Xmega document below
http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf
//TCC0 is on 0x0800 (Port C counter 0)

1. Use an internal clock source with prescaler factor of 1024 in the Control Register A(CTRLA)
//write 0x05 to clkselect timer+0x00 for prescalar 1024 p175

1. Use the Counter Register Low(CNTL) to store the expected delay you want. 
This timer will count up, so make sure you subtract the expected delay count from the maximum count 
value that can be stored. Set the Counter Register Hi(CNTH) to 0xFF, since we are only 
'using the low 8 bits for our delay routine.
//CNTL is +0x20, CNTH is 0x21
4. Use the OVFIF bit in the Interrupt Flag Register(INTFLAGS) to check for the overflow condition, 
this occurs when the timer reaches the expected delay count
//+0x0C is the int flag register, bit 0 is OVFIF
//can clear by writing a 1 to this location or with interrupt vector
5. Update the devreg.h header file with the memory mapped registers for the is timer.
6. Hint: Make sure to clear the OVFIF bit each time the overflow condition occur

	
*/


#include <devreg.h>

// From module: Interrupt management - XMEGA implementation
#include <interrupt.h>

//protos
//LEDs
void blink(unsigned *, unsigned *,unsigned);
unsigned swStateCheck(unsigned);
void toggleLED(unsigned);

//registers
void WriteReg(unsigned, unsigned char );
unsigned ReadReg(unsigned);

//Timers
void initTimer(unsigned);
void counterDelay(unsigned);
void repeatCounterDelay(unsigned);

//debug protos
void debugBlink(unsigned );


//Software Polling Delays
//void HmSDelay(void);
//void Delay(unsigned);

//interrupts
//void enableInt(void);


int main (void)
{				
	while(1){		
		//blinking round robin
		unsigned sw1 = 0;
		unsigned sw2 = 0;
		//round robin polling
		//check switch 1
		sw1 = swStateCheck(1);
		
		//check switch 2
		sw2 = swStateCheck(2);
		
		blink(&sw1,&sw2,6);
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
			repeatCounterDelay(delay);
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
	
	if((swState & swMask[swNum]) == swMask[swNum]){
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

//write Value to RegAddress 
void WriteReg(unsigned regAddress, unsigned char Value){
	*((volatile unsigned char *)regAddress) = Value;
}

//read value in Regaddress and return 
unsigned ReadReg(unsigned regAddress){
	return *((volatile unsigned char *)regAddress);
}

//initialize a timer with 1024 scaling only low
//generalize 1024?
void initTimer(unsigned timer){
	//1024 scaling
	WriteReg(timer, PRESCALAR_1024);
	//set upper bit to all 1s since just using low
	WriteReg((timer | TIMER_HIGH), 0xff );
	//enable overflow interrupt
	WriteReg(timer | TIMER_INT_ENABLE,0x03);
	//clear the ovfif bit
	WriteReg(timer|TIMER_OVFIF,0x01);
}

//implement a delay using the timer counter 
//utilizes portc timer 0
//max delay 255 == ~0.15
void counterDelay(unsigned delay){
	initTimer(PORTC_TIMER0);
	
	//max delay 255
	WriteReg(PORTC_TIMER0|TIMER_LOW,(255-delay));
	
	//catch here until overflow hits
	while((ReadReg(PORTC_TIMER0|TIMER_OVFIF)&0x01)==0){
				
	}
}

//repeats the full amount of lowbit timer to get higher delay times
//1 full timer ~ 0.12s, 4 repeats ~0.5s
void repeatCounterDelay(unsigned numRepeats){
	for(int i=0;i<numRepeats;i++){
		counterDelay(255);
	}
}


//Software Polling Delays

//should delay about 100ms
/*
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
*/
/*
//global interrupt enable
void enableInt(){
	unsigned status = ReadReg(CPU_STATUS);
	WriteReg(CPU_STATUS, status ^ 0x80 );
}
*/

//blink led if line reached
//for debug only
/*
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
*/
