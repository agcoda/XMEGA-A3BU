/*
 * devreg.h
 *
 * Created: 10/10/2023 8:17:17 PM
 *  Author: Alex Gauf
 */ 


#ifndef DEVREG_H_
#define DEVREG_H_

//CPU
#define CPU_REG				0x0030
#define CPU_STATUS			CPU_REG & 0x0F		//Bit 7 is global interrupt enable

//Ports
#define PORTR_DATA			0x07E0		//PORT B Data Direction
#define PORTR_OUTPUT		0x07E7		//Address for setting LED1 and LED0
#define PORT_OUTPUT			0x7

#define PORTE_DATA			0x0680		
#define PORTE_INPUT			0x0688

#define PORTF_DATA			0x06A0
#define PORTF_INPUT			0x06A8

#define PORTC_DATA			0x0640

//Timers
#define PORTC_TIMER0		0X0800					//also ClkSelect

#define TIMER_LOW			0x20					//add this to timer base to access lowbit
#define TIMER_HIGH			0x21
#define TIMER_OVFIF			0x0C					//low bit overflow flag , write 1 to bit 1 to clear it
#define TIMER_INT_ENABLE	0x06

//interrupt levels enum HIGH, MED, LO, OFF
#define INT_HIGH			0x03
#define INT_LOW				0x01






#define PRESCALAR_1024		0x07					//write this to clkSelect
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