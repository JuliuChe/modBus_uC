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
uint8_t temp_CRC;
// Current position pointer for storing receive position
uint8_t recPtr = 0;

void modbus_timer(void)
{
	    TMR0_StopTimer();
        modbus_analyse_and_answer();

   
}

uint8_t modbus_analyse_and_answer(void)
{
    index = eusart1RxCount;
    
    for(int i=0; i<index; i++)
    {
    rx_buf[i]=EUSART1_Read();
    }
    uint8_t length =0;
    uint16_t addressReg=0;
    uint16_t numReg=0;
     uint16_t valReg=0;
    uint8_t* ptrTxBuf;

   
    if(rx_buf[0]==modbusAddress){
        if((rx_buf[index-1]<<8 | rx_buf[index-2])==CRC16(rx_buf, (index-2)))
        {
            tx_buf[0]=modbusAddress;
            switch(rx_buf[1])
            {
                case READ_INPUT_REGISTERS:
                {
                    addressReg=((rx_buf[2]<<8)|rx_buf[3]);
                    numReg = ((rx_buf[4]<<8)|rx_buf[5]);
                    
                    if((numReg>=2 && addressReg>=1) || numReg<1){
                    //error, number of registers is too high  
                     tx_buf[1]=0x84;
                     tx_buf[2]=ILLEGAL_DATA_VALUE;
                     length=3;
                    }
                    else if(addressReg>1 ||addressReg<0) 
                    {
                    //error starting address is out of bound
                     tx_buf[1]=0x84;
                     tx_buf[2]=ILLEGAL_DATA_ADDRESS;
                     length=3;
                             
                    }
                    else
                    {
                    tx_buf[1]=READ_INPUT_REGISTERS;
                    tx_buf[2]=(2*numReg);
                    ptrTxBuf=&tx_buf[3];
                            length +=3;
                            
                    for(int i=(addressReg); i<(addressReg+numReg);i++)
                    {
                        (*ptrTxBuf)=(input_registers[i]>>8);
                        ptrTxBuf++;
                        (*ptrTxBuf)=(input_registers[i]);
                        ptrTxBuf++;
                        length+=2;
                    }
                    }
                    
                    
                            modbus_send(length);
                }
                            break;
                            
                case READ_HOLDING_REGISTERS:
                    {
                    addressReg=((rx_buf[2]<<8)|rx_buf[3]);
                    numReg = ((rx_buf[4]<<8)|rx_buf[5]);
                    
                    if(numReg!=1){
                    //error, number of registers is too high  
                     tx_buf[1]=0x83;
                     tx_buf[2]=ILLEGAL_DATA_VALUE;
                     length=3;
                    }
                    else if(addressReg!=0) 
                    {
                    //error starting address is out of bound
                     tx_buf[1]=0x83;
                     tx_buf[2]=ILLEGAL_DATA_ADDRESS;
                     length=3;
                    }
                    else
                    {
                    tx_buf[1]=READ_HOLDING_REGISTERS;
                    tx_buf[2]=(2*numReg);
                    ptrTxBuf=&tx_buf[3];
                    length +=3;
                            
                    for(int i=(addressReg); i<(addressReg+numReg);i++)
                    {
                        (*ptrTxBuf)=(holding_registers[i]>>8);
                        ptrTxBuf++;
                        (*ptrTxBuf)=(holding_registers[i]);
                        ptrTxBuf++;
                        length+=2;
                    }
                    }      
                     modbus_send(length);
                }
                    
                    break;
                case WRITE_SINGLE_REGISTER:
                      {
                    addressReg=((rx_buf[2]<<8)|rx_buf[3]);
                    valReg = ((rx_buf[4]<<8)|rx_buf[5]);
                    holding_registers[0]=valReg;
                    if(addressReg==0)
                    {
                    
                    tx_buf[1]=WRITE_SINGLE_REGISTER;
                    tx_buf[2]=0;
                    tx_buf[3]=addressReg;    
                    tx_buf[4]=(holding_registers[0]>>8);
                    tx_buf[5]=(holding_registers[0]);
                    length=6;
                     }
                    else
                    {
                    tx_buf[1]=0x86;
                    tx_buf[2]=ILLEGAL_DATA_ADDRESS;
                    length=3;
                    }
                    modbus_send(length);
                }
                    break;
                default:
                {
                    tx_buf[1]=((rx_buf[1])|0x80);
                    tx_buf[2]=ILLEGAL_FUNCTION;
                    length=3;
                    modbus_send(length);

                }
                break;
                    
                    
            }
        }
    }
}
 
//return size_of_answer  


void modbus_char_recvd(uint8_t c)
{
//	TMR0 reset 
}

void modbus_send(uint8_t length)
{
	uint16_t temp16; 
	uint8_t i=0;

	// TODO -> complete modbus RCR calculation
    
    uint8_t* ptrTxBuf=&tx_buf[length];
    
    temp16= CRC16(tx_buf, length);
    (*ptrTxBuf) = temp16;
    ptrTxBuf++;
    (*ptrTxBuf)=temp16>>8;
                    
	length += 2; // add 2 CRC bytes for total size
    ptrTxBuf=tx_buf;
    for(i;i<length;i++)
    {
    if(EUSART1_is_tx_ready())
    {
    EUSART1_Write(*ptrTxBuf);
    }
    ptrTxBuf++;
    }
    temp16= CRC16(tx_buf, length);
    
	// For all the bytes to be transmitted
 // uart_send(tx_buf,length);
}

void modbus_init(uint8_t address)
{
	modbusAddress = address;
  // TODO -> configure timer for modbus usage
    
}