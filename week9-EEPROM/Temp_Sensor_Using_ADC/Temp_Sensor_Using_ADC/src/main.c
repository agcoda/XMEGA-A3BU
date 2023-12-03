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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "stdio.h"
#include "adc_sensors.h"
#include "ntc_sensor.h"
#include "sysfont.h"
#define IN_F  0x6A8 //Data IN Register, Port F
#define DES_TEMP_EEMEM 0
#define FIRSTRUN_EEMEM DES_TEMP_EEMEM+1

//! The X position of the temperature image on screen
#define X_POS 10

//! Flash text for the heading
PROGMEM_DECLARE(char const, header[]) = "Temperature(F)";

//! The thermometer image
uint8_t tempscale_data[] = {
	0x01, 0xf9, 0xfd, 0xfd, 0xf9, 0x01,
	0x41, 0xff, 0xff, 0xff, 0xff, 0x41,
	0x10, 0xff, 0xff, 0xff, 0xff, 0x10,
	0x9e, 0xbf, 0xbf, 0xbf, 0xbf, 0x9e
};

 
int main (void)
{
	
	
	
	// Bitmap to hold the actual thermometer image
	struct gfx_mono_bitmap tempscale;
	// String to hold the converted temperature reading
	char temperature_string[15];
	// Variable to hold the image thermometer scale
	uint8_t temp_scale;
	
	
   // Variables for holding the current and desired temperatures
   int16_t current_temperature; 
   int16_t desired_temperature= nvm_eeprom_read_byte(DES_TEMP_EEMEM);
   bool desTempUpdateFlag = false;
   
    // Intialize board here
	sysclk_init();
	board_init();
	pmic_init();
	gfx_mono_init();
	adc_sensors_init();
	
	

	// Enable display backlight
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	cpu_irq_enable();

	// Struct for holding the temperature scale background
	tempscale.type = GFX_MONO_BITMAP_RAM;
	tempscale.width = 6;
	tempscale.height = 32;
	tempscale.data.pixmap = tempscale_data;

	// Screen border
	gfx_mono_draw_rect(0, 0, 128, 32, GFX_PIXEL_SET);
	// Clear screen
	gfx_mono_draw_filled_rect(1, 1, 126, 30, GFX_PIXEL_CLR);

	// Draw the header
	gfx_mono_draw_progmem_string((char PROGMEM_PTR_T)header, 27, 2, &sysfont);
    // Draw the thermometer
    gfx_mono_put_bitmap(&tempscale, X_POS, 0);


//------------------------------------------
//START Added Code Part 1/2 agauf 12/3
//------------------------------------------
	
	//eeproms defaults to 0xff or 0 if wiped so the first time this is ever run we need to check for that and set a default desired.
	
	if(nvm_eeprom_read_byte(FIRSTRUN_EEMEM)==255 || nvm_eeprom_read_byte(FIRSTRUN_EEMEM)==0){
		desired_temperature = (read_temperature()*1.8+32);
		nvm_eeprom_write_byte(FIRSTRUN_EEMEM,(uint8_t)1);
	}


//------------------------------------------
//END Added Code Part 1/2 agauf 12/3 
//------------------------------------------
	
	
	while(1)
	{
		//get current temperature	
		current_temperature = read_temperature();	
	
		// Convert the temperature into the thermometer scale
		temp_scale = -0.36 * current_temperature + 20.25;
		if (temp_scale <= 0) {
			temp_scale = 0;
		}

		// Draw the scale element on top of the background temperature image
		gfx_mono_draw_filled_rect(X_POS + 2, 3, 2, temp_scale,
		GFX_PIXEL_CLR);
	 
		// Convert the temperature to Fahrenheit
		current_temperature = current_temperature * 1.8 + 32;
		snprintf(temperature_string, sizeof(temperature_string),
		"%3i Current", current_temperature);

		// Draw the Fahrenheit temperature string
		gfx_mono_draw_string(temperature_string, X_POS + 12, 21, &sysfont);
    
		if( desired_temperature == 0)
			desired_temperature = current_temperature;

		snprintf(temperature_string, sizeof(temperature_string), "%3i Desired",
		desired_temperature);

		// Draw the Desired string
		gfx_mono_draw_string(temperature_string, X_POS + 12, 13, &sysfont);
/*
		//read LED switches
		if ( gpio_pin_is_low(GPIO_PUSH_BUTTON_1) )
			desired_temperature++;
  
		if ( gpio_pin_is_low(GPIO_PUSH_BUTTON_2) )
			desired_temperature--;

		if ( desired_temperature == current_temperature)
		{
			LED_Off(LED0);  // Turn AC and Heater off
			LED_Off(LED1);
		}
		else if ( desired_temperature < current_temperature)
		{
			LED_On(LED0);  // Turn AC on
			LED_Off(LED1);
		}
		else
		{
			LED_Off(LED0);
			LED_On(LED1); // Turn Heater on
		}
*/	
/*
Add the following logic to the existing code.	
	Use LED_off() and LED_on() functions to control LED0 and LED1
	Use gpio_pin_is_low() function to check GPIO_PUSH_BUTTON_1 and GPIO_PUSH_BUTTON_2 state

	1. Set Desired temperature using switch switch1 to increase Desired temperature,
	   and switch2 to decrease Desired temperature	 
	2. If current temperature is equal to desired temperature, both LED0 and LED1 are off
	3. If current temperature is above desired temperature, turn LED0 on, LED1 off
	4. If current temperature is below desired temperature, turn LED0 off, LED1 on
*/	

//------------------------------------------
//START Added Code agauf 11/11
//------------------------------------------
		//no timing requirements for button response in spec
		//Have to be holding the button when loop hits this part
		//Temp adjust is a bit slow to respond since the draw takes awhile
		
		//Improve with button ISR, or task prio 
	
		/*
		1. Set Desired temperature using switch switch1 to increase Desired temperature,
		and switch2 to decrease Desired temperature
		*/
		//if sw 1 depressed inc desired temp
		if(gpio_pin_is_low(GPIO_PUSH_BUTTON_1)){
			desired_temperature+=1;
			desTempUpdateFlag = true;
		}
		else if(gpio_pin_is_low(GPIO_PUSH_BUTTON_2)){
			desired_temperature-=1;
			desTempUpdateFlag = true;
		}
		
		
//------------------------------------------
//START Added Code Part 2/2 agauf 12/3
//------------------------------------------
		//whenever temp is updated store it to eeprom in case power failure
		if(desTempUpdateFlag){
			nvm_eeprom_write_byte(DES_TEMP_EEMEM, (uint8_t)desired_temperature);
			desTempUpdateFlag = false;
		}


//------------------------------------------
//END Added Code Part 2/2 agauf 12/3
//------------------------------------------
		
		/*
		2. If current temperature is equal to desired temperature, both LED0 and LED1 are off
		3. If current temperature is above desired temperature, turn LED0 on, LED1 off
		4. If current temperature is below desired temperature, turn LED0 off, LED1 on
		*/
		if(current_temperature>desired_temperature){
			LED_On(LED0);
			LED_Off(LED1);
		}
		else if(current_temperature<desired_temperature){
			LED_Off(LED0);
			LED_On(LED1);
		}
		else{
			LED_Off(LED0);
			LED_Off(LED1);
		}
//------------------------------------------
//END Added Code agauf 11/11
//------------------------------------------

		

	}
	
}
