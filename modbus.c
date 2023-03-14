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
uint16_t index = 0;

void modbus_timer(void)
{
    TMR0_StopTimer();
    modbus_analyse_and_answer();


}

uint8_t modbus_analyse_and_answer(void)
{
    uint8_t length = 0;
    uint16_t addressReg = 0;
    uint16_t numReg = 0;
    uint16_t valReg = 0;
    uint8_t* ptrTxBuf; //used only when we have to go over several input registers


    if (rx_buf[0] == modbusAddress)
    {
        if ((rx_buf[index-1] << 8 | rx_buf[index - 2]) == CRC16(rx_buf, (index - 2)))
        {
            tx_buf[0] = modbusAddress; //Whatever happens, the first answer from the client is its adress on the bus (i.e. 0x84) 
            switch (rx_buf[1]) // rx_buf[1] corresponds to the modbus function specified by the server
            {
            case READ_INPUT_REGISTERS: //Read input registers, could be 1 or 2 value @ two different address (0 and 1))
            {
                addressReg = ((rx_buf[2] << 8) | rx_buf[3]); //Adresss of the first register
                numReg = ((rx_buf[4] << 8) | rx_buf[5]); //16 bits word corresponding to the number of registers following addressReg

                if (((numReg > 2)  || (numReg < 1))||((numReg==2) && (addressReg==1)))
                {
                    //error, number of registers is incorrect  
                    tx_buf[1] = 0x84;
                    tx_buf[2] = ILLEGAL_DATA_VALUE;
                    length = 3;
                }
                else if (addressReg > 1 || addressReg < 0)
                {
                    //error starting address is out of bound
                    tx_buf[1] = 0x84;
                    tx_buf[2] = ILLEGAL_DATA_ADDRESS;
                    length = 3;

                }
                else
                {
                    //That is not an error, here we give a response to the query sent by the server    
                    tx_buf[1] = READ_INPUT_REGISTERS; //modbus function performed i.e.0x04
                    tx_buf[2] = (2 * numReg); //2*N in 1byte word
                    ptrTxBuf = &tx_buf[3];
                    length += 3;

                    //We loop over the number of registers specified by numReg starting at addressReg         
                    for (int i = (addressReg); i < (addressReg + numReg); i++)
                    {
                        (*ptrTxBuf) = (input_registers[i] >> 8);
                        ptrTxBuf++;
                        (*ptrTxBuf) = (input_registers[i]);
                        ptrTxBuf++;
                        length += 2;
                    }
                }

                //Whatever we have in tx_buf, we send it back to the server
                modbus_send(length);
            }
                break;

            case READ_HOLDING_REGISTERS:
            {
                addressReg = ((rx_buf[2] << 8) | rx_buf[3]);
                numReg = ((rx_buf[4] << 8) | rx_buf[5]);

                if (numReg != 1)
                {
                    //error, number of registers is incorrect, we can only read 1 register  
                    tx_buf[1] = 0x83;
                    tx_buf[2] = ILLEGAL_DATA_VALUE;
                    length = 3;
                }
                else if (addressReg != 0)
                {
                    //error starting address is out of bound, there is only adress 0 that can be read
                    tx_buf[1] = 0x83;
                    tx_buf[2] = ILLEGAL_DATA_ADDRESS;
                    length = 3;
                }
                else
                {
                    //That is not an error, here we give a response to the query sent by the server     
                    tx_buf[1] = READ_HOLDING_REGISTERS;
                    tx_buf[2] = (2 * numReg);
                    tx_buf[3] = (holding_registers[addressReg] >> 8);
                    tx_buf[4] = (holding_registers[addressReg]);
                    length = 5;
                }
            }
                modbus_send(length);
            }

            break;
        case WRITE_SINGLE_REGISTER:
            {
                addressReg = ((rx_buf[2] << 8) | rx_buf[3]);
                valReg = ((rx_buf[4] << 8) | rx_buf[5]);
               
                if (addressReg == 0)
                {
                     holding_registers[addressReg] = valReg;
                    //Normal well formatted modbus query
                    tx_buf[1] = WRITE_SINGLE_REGISTER;
                    tx_buf[2] = 0;
                    tx_buf[3] = addressReg;
                    tx_buf[4] = (holding_registers[0] >> 8);
                    tx_buf[5] = (holding_registers[0]);
                    length = 6;
                }
                else
                {
                    //Error, address ill-specified
                    tx_buf[1] = 0x86;
                    tx_buf[2] = ILLEGAL_DATA_ADDRESS;
                    length = 3;
                }
                modbus_send(length);
            }
            break;
        default:
            {
                //That case is executed only if the modbus function is not recognized and throw back an ILLEGAL FUNCTION error code to the server.
                tx_buf[1] = ((rx_buf[1]) | 0x80);
                tx_buf[2] = ILLEGAL_FUNCTION;
                length = 3;
                modbus_send(length);

            }
            break;


        }
    }
}


//return size_of_answer  

void modbus_char_recvd(uint8_t c)
{
    //	TMR0 reset and restart
    TMR0_Reload();
    TMR0_StartTimer();
    rx_buf[index] = c;//Store the last character in the rx buffer
    index++;//go to the next slot in the table

}

void modbus_send(uint8_t length)
{
    uint16_t temp16;
    uint8_t i = 0;
    uint8_t* ptrTxBuf = &tx_buf[length];

    //calculate the 2bytes word CRC then store it in the Tx buffer
    temp16 = CRC16(tx_buf, length); 
    (*ptrTxBuf) = temp16;
    ptrTxBuf++;
    (*ptrTxBuf) = temp16 >> 8;
    length += 2; // add 2 CRC bytes for total size
    
    //redirect the pointer to the beginning of the rx_buf table
    ptrTxBuf = tx_buf;
    //loop over the entire buffer and send each character to the EUSART_Tx
    for (i; i < length; i++)
    {
        if (EUSART1_is_tx_ready())
        {
            EUSART1_Write(*ptrTxBuf);
        }
        ptrTxBuf++;
    }
    // For all the bytes to be transmitted
    // uart_send(tx_buf,length);
}


//Set the address of the client to address
void modbus_init(uint8_t address)
{
    modbusAddress = address;
    // TODO -> configure timer for modbus usage

}