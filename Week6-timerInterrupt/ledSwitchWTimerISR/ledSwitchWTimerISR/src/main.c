/*
 * main.c
 *
 * Created: 10/24/2023 
 *  Author: Alex Gauf
 */ 

/* Assignment 5
1. Use the Xmega timer 0(TCC0) hardware to create delay code by using the
timer interrupt. Use the Programming assignment #4 and instead of polling
for the overflow bit use the interrupt to change the state of a variable
between 0 and 1 when ever the timer expires. In the main code instead of
polling for the overflow bit, poll for the state of the variable to be 0, exit the
polling when the variable changes from 0 to 1, which will happen when the
next interrupt happens.

2. Use the ASFWizard to include the Timer Counter Module.
Search for the “xmega_tc_qs_ovf” for the Over flow use case steps

3. Make sure you make the interrupt variable a volatile

4. When using the interrupt method, you only need to initialize the timer
once in the main function.
*/


#include <devreg.h>
#include <asf.h>

//protos
//LEDs
void blink(unsigned *, unsigned *,unsigned);
unsigned swStateCheck(unsigned);
void toggleLED(unsigned);

//registers
void writeReg(unsigned, unsigned char );
unsigned readReg(unsigned);

//Timers
void initTimer(unsigned);

//debug protos
void debugBlink(unsigned );

//timer interrupt delay
void delay(unsigned);

//interrupts

//interrupt data
volatile unsigned timeUp = 0;

//ISR for timer 0 interrupt
static void my_callback(void)
{
	timeUp = 1;
	tc_clear_overflow(&TCC0);
}


int main (void)
{				
	
	//inits required for timer driver
	//see tc.h l1407
	//interrupt controller
	pmic_init();
	//system clock start
	sysclk_init();
			
	while(1){		
		
		//blinking round robin		
		unsigned sw1 = 0;
		unsigned sw2 = 0;
		//round robin polling
		//check switch 1
		sw1 = swStateCheck(1);
		
		//check switch 2
		sw2 = swStateCheck(2);
		
		blink(&sw1,&sw2,1500);
		
	}
}

//blink leds 1,0 based on 1/0 in referenced for delay about 1ms per ct (1500 ~= 13.79s)
void blink(unsigned *p_sw1, unsigned *p_sw2, unsigned delayCount){
		
		//if either or both button pressed blink on off before checking states again
		//if neither pressed still blink both
		for(int i=0;i<2;i++){
			if(*p_sw1==1 || *p_sw2==1){
				if(*p_sw1==1){
					toggleLED(LED0_N);
				}
				if(*p_sw2==1){
					toggleLED(LED1_N);
				}
			}
			else{
				toggleLED(LED0_N);
				toggleLED(LED1_N);
			}
			delay(delayCount);
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
		writeReg(portData[swNum],0x00);
		portInit[swNum] =0;
	}
	
	//When a button is pressed it will drive the I/O line to GND
	//so the needed bit will be low
	swState = ~readReg(portIn[swNum]);
	
	//debugBlink(swState); 
	
	if((swState & swMask[swNum]) == swMask[swNum]){
		return 1;
	}
	else{
		return 0;
	}
	
}

//ledx toggle on or off
void toggleLED(unsigned ledNum){
	//first call needs to set portR as output then init the output array
	static unsigned init = 1;
	if(init == 1){
		writeReg(PORTR_DATA,0xff);
		writeReg(PORTR_DATA | PORT_OUTPUT,0xff);
		init =0;
	}
	
	//writing 1 to the appropriate ledByte TOGGLES the LED
	switch(ledNum){
	case LED0_N:
		writeReg(PORTR_DATA | PORT_OUTPUT, BIT0_MASK);
		break;
	case LED1_N:		
		writeReg(PORTR_DATA | PORT_OUTPUT, BIT1_MASK);
		break;
	}
	
}

//write Value to RegAddress 
void writeReg(unsigned regAddress, unsigned char Value){
	*((volatile unsigned char *)regAddress) = Value;
}

//read value in Regaddress and return 
unsigned readReg(unsigned regAddress){
	return *((volatile unsigned char *)regAddress);
}

//initialize a timer for tc.h
void initTimer(unsigned count){
	//inits the timer enc
	tc_enable(&TCC0);
	//assigns ISR for the ovf interrupt
	tc_set_overflow_interrupt_callback(&TCC0, my_callback);
	//set incrementing counter mode
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	//set counter TOP when ovf triggers
	tc_write_period(&TCC0, count);
	//set interrupt priority level
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	//enable interrupts
	cpu_irq_enable();
	//set source and start counting
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1024_gc);
}

//delay by running a timer until it throws interrupt at set delay count
void delay(unsigned count)
{
	//reset the ISR flag
	timeUp =0;
	//start timer
	initTimer(count);
	//wait until ISR callback changes timeUp to 1
	while(timeUp == 0);
}


//blink led if line reached
//for debug only
/*
void debugBlink(unsigned times){
	for(int i=0;i<times;i++){
		for(int i=0;i<2;i++){

			toggleLED(LED0_N);
			toggleLED(LED1_N);
			delay(1);
		}	
}
delay(2);
}
*/

