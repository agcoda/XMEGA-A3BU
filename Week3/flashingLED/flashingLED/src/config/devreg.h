/*
 * devreg.h
 *
 * Created: 10/10/2023 8:17:17 PM
 *  Author: Alex Gauf
 */ 


#ifndef DEVREG_H_
#define DEVREG_H_

//Ports
#define PORTB_DATA			0x07E0		//PORT B Data Direction
#define PORTB_LED_STATE		0x07E7		//Address for controlling LED1 and LED0

//
#define H_MS_DELAY_CT 11160			//each read is about 9 us from manual timing so 11160 reads is 100 ms.

#endif /* DEVREG_H_ */