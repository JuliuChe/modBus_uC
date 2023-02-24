/************************************************************************//**
 * \file	main.c
 * \brief	Program main file for MODBUS controller
 ***************************************************************************/


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "lcd/lcd.h"
#include "pwm.h"
#include "measure.h"
#include "modbus.h"
#include "uart.h"

#define _XTAL_FREQ  64000000L
/**
 * Initialize clock, buttons, leds and interrupts
 */
void init_hw()
{
	OSCCON3 = 0; // runs at 64MHz
	LCD_Init(LCD_2x16);
	adc_init();
	modbus_init(0x80);
	pwm_init();
  uart_init();
	// Interrupts configuration
	PEIE = 1;
	GIE = 1;
}

uint8_t tmpStr[30];
uint16_t u; ///< Voltage in milliovolts
uint16_t i; ///< Current in microamperes
uint16_t p; ///< Power in microwatts

uint16_t offset;
uint16_t pwm = 512;

void main(void)
{
	init_hw();
	
	LCD_2x16_WriteMsg((unsigned char *)"Welcome !       ", 0); // display on line 0
	LCD_2x16_WriteMsg((unsigned char *)"*-*-*-*-*-*-*-*-", 1); // display on line 1
			
	// TODO -> complete measure of current offset

	while (true) {
	// TODO -> complete the main loop
	}
}