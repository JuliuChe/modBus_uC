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
        TMR0_Reload();
        TMR0_StartTimer();
   
}

uint8_t modbus_analyse_and_answer(void)
{
    index = eusart1RxCount;
    
    for(int i=0; i<index; i++)
    {
    rx_buf[i]=EUSART1_Read();
    }
    uint8_t length =0;
    if(rx_buf[0]==0x80){
        if((rx_buf[index-1]+rx_buf[index-2])==CRC16(rx_buf, (index-2))
        {
            switch(*rx_buf)
            {
                case READ_INPUT_REGISTERS:
                    uint16_t address=(rx_buf[1]<<8)+rx_buf[2];
                    uint16_t numReg = (rx_buf[3]<<8)+rx_buf[4];
                    tx_buf[0]=READ_INPUT_REGISTERS;
                    if(numReg<128){
                    tx_buf[1]=(2*numReg);
                    }
                    uint8_t* ptrTxBuf=tx_buf[2];
                            length +=2;
                    for(int i=0; i<numReg;i++)
                    {
                        (*ptrTxBuf)=(input_registers[i]>>8);
                        ptrTxBuf++;
                        (*ptrTxBuf)=input_registers[i]<<8;
                        ptrTxBuf++;
                        length+=2;
                    }
                            modbus_send(length);
                    break;
                case READ_HOLDING_REGISTERS:
                    break;
                case WRITE_SINGLE_REGISTER:
                    break;
                    
                    
            }
        }
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
    uint8_t* ptrTxBuf=tx_buf[length];
     temp16= CRC16(tx_buf, length);
                    (*ptrTxBuf) = crc>>8;
                    ptrTxBuf++;
                    (*ptrTxBuf)=crc<<8;
	length += 2; // add 2 CRC bytes for total size
    uart_send(tx_buf, length);
	// For all the bytes to be transmitted
 // uart_send(tx_buf,length);
}

void modbus_init(uint8_t address)
{
	modbusAddress = address;
  // TODO -> configute timer for modbus usage
    
}