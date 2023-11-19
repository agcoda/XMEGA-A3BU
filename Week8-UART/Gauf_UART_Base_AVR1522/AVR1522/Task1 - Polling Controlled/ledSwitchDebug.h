/*
 * ledSwitchDebug.h
 *
 * Created: 11/19/2023 2:16:06 PM
 *  Author: Alex Gauf
 */ 


#ifndef LEDSWITCHDEBUG_H_
#define LEDSWITCHDEBUG_H_



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
		for(int j=0;j<2;j++){

			toggleLED(LED0);
			toggleLED(LED1);
			Delay(1);
		}
	}
	Delay(10);
}



#endif /* LEDSWITCHDEBUG_H_ */