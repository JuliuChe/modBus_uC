/*
 * File:   lcd.c
 * Author: pascal.sartoret
 *
 * Created on 16. décembre 2020, 14:04
 */

#define _XTAL_FREQ 25000000L

#include <xc.h>
#include <string.h>
#include "lcd.h"
#include "ssd1963_cmd.h"



int8_t Lcd_Init(void) 
{
        //--------------------------------------------------------------------------
        // define GPIO for LCD
        DIR_LCD_RS = 0;         // RS is an output
        LCD_RS = 0;             // command mode
        DIR_LCD_CS = 0;         // chip select is an output
        LCD_CS = 0;             // do not select the LCD
        LCD_DATA_L_DIR = LCD_DATA_L_DIR & 0x0F;  // data port is 4 bits output
    
        __delay_ms(200);  // power on delay
        LCD_2x16_WriteCmd(0x02);
        
        LCD_2x16_WriteCmd(0x28);
//        LCD_2x16_WriteCmd(0x2B);
//        LCD_2x16_WriteCmd(0x2B);
        LCD_2x16_WriteCmd(0x01);    // clear display 
        LCD_2x16_WriteCmd(0x0C);
//        LCD_2x16_WriteCmd(0x14);
//        __delay_ms(2);
        LCD_2x16_WriteCmd(0x06);
//        __delay_ms(100);        
        return 0;
}


void LCD_2x16_WriteCmd(uint8_t command)
{
    LCD_RS = 0;             // command mode
    LCD_DATA_L = (command & 0xF0) | (LCD_DATA_L & 0x0F);   // bits to set
    LCD_CS = 1;             // chip select
    __delay_us(10);
    LCD_CS = 0;             // chip deselect
    __delay_us(1);
    command = command << 4;
    LCD_DATA_L = (command & 0xF0) | (LCD_DATA_L & 0x0F);   // bits to set
    LCD_CS = 1;             // chip select
    __delay_us(10);
    LCD_CS = 0;             // chip deselect
    __delay_ms(3);
}
void LCD_2x16_WriteData(uint8_t command)
{
    LCD_RS = 1;             // command mode
    LCD_DATA_L = (command & 0xF0) | (LCD_DATA_L & 0x0F);   // bits to set
    LCD_CS = 1;             // chip select
    __delay_us(1);
    LCD_CS = 0;             // chip deselect
    __delay_us(1);
    command = command << 4;
    LCD_DATA_L = (command & 0xF0) | (LCD_DATA_L & 0x0F);   // bits to set
    LCD_CS = 1;             // chip select
    __delay_us(1);
    LCD_CS = 0;             // chip deselect
    __delay_us(100);
}

void LCD_2x16_WriteMsg(unsigned char * msg, uint8_t line)
{
    LCD_2x16_WriteCmd(0x80 | (line << 6));
    do
    {
        LCD_2x16_WriteData(*msg);
        msg++;
    }while(*msg != 0);
}