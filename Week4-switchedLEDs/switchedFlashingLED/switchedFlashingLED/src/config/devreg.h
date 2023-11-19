/*
 * devreg.h
 *
 * Created: 10/10/2023 8:17:17 PM
 *  Author: Alex Gauf
 */ 


#ifndef DEVREG_H_
#define DEVREG_H_

//Ports
#define PORTR_DATA				0x07E0		//PORT B Data Direction
#define PORTR_OUTPUT			0x07E7		//Address for setting LED1 and LED0

#define PORTE_DATA				0x0680		
#define PORTE_INPUT				0x0688

#define PORTF_DATA				0x06A0
#define PORTF_INPUT				0x06A8
//LEDs
#define LED0 0
#define LED1 1
//Toggle Masks
#define BIT0_MASK 0x01
#define BIT1_MASK 0x02
#define BIT2_MASK 0x04
#define BIT5_MASK 0x10

//
#define H_MS_DELAY_CT 5500			//each read is about 9 us from manual timing so 11160 reads is 100 ms.

#endif /* DEVREG_H_ */