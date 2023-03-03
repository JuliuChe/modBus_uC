#include "modbus.h"
#include "mcc_generated_files/mcc.h"
#include "crc.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

// Modbus functions
#define READ_INPUT_REGISTERS    0x04
#define READ_HOLDING_REGISTERS  0x03
#define WRITE_SINGLE_REGISTER   0x06

// Modbus data model
uint8_t modbusAddress;
uint16_t input_registers[2];
uint16_t holding_registers[2];

// Modbus error codes
#define ILLEGAL_FUNCTION		1
#define ILLEGAL_DATA_ADDRESS	2
#define ILLEGAL_DATA_VALUE		3
#define SLAVE_DEVICE_FAILURE	4

/**
 * Buffers for serial receive and send operations 
 * which are more than one byte long
 **/
uint8_t rx_buf[256];
uint8_t tx_buf[256];
uint16_t index=0;

// Current position pointer for storing receive position
uint8_t recPtr = 0;

void modbus_timer(void)
{
	    TMR0_StopTimer();
        modbus_analyse_and_answer();
        TMR0_StartTimer();
        TMR0_Reload();
   
}

uint8_t modbus_analyse_and_answer(void)
{
    index = eusart1RxCount;
    
    for(int i=0; i<index; i++)
    {
    rx_buf[i]=EUSART1_Read();
    }
    if(rx_buf[0]==0x80){
        
       //Check if CRC is correct using function :  CRC16(rx_buf, index)
       //swap 
       //Switch Case for each function : 03, 04, etc....
        

        
    }
 
//return size_of_answer  
}

void modbus_char_recvd(uint8_t c)
{
//	TMR0 reset 
}

void modbus_send(uint8_t length)
{
	uint16_t temp16; 
	uint8_t i;

	// TODO -> complete modbus RCR calculation
	length += 2; // add 2 CRC bytes for total size

	// For all the bytes to be transmitted
 // uart_send(tx_buf,length);
}

void modbus_init(uint8_t address)
{
	modbusAddress = address;
  // TODO -> configute timer for modbus usage
}