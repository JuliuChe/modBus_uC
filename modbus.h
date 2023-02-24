/************************************************************************//**
 * \file	modbus.h
 * \brief	Modbus serial library
 ***************************************************************************/

#ifndef MODBUS_H
#define	MODBUS_H

#include <stdint.h>

extern uint8_t modbusAddress;
extern uint16_t input_registers[2];
extern uint16_t holding_registers[2];

/**
 * Initializes this Modbus library. 
 * 
 * Initialize pins, timer, serial port and variables. Interrupts 
 * MUST be initialized for the  timer and the serial port.
 * 
 * @param address : the Modbus address of this device
 * 
 * @see modbus_char_recvd
 * @see modbus_timer
 * @see interrupts.c
 */
void modbus_init(uint8_t address);

/**
 * Analyses the received frame and answer to server
 * 
 * @return an error code if frame not valid
 */
uint8_t modbus_analyse_and_answer(void);

/**
 * Send the Modbus frame on tx_buf with CRC added
 * 
 * @param length : length of the frame without the CRC 
 */
void modbus_send(uint8_t length);

/**
 *  
 * This function will be called every time a character has been received on the
 * Modbus serial port. It is called from the interrupt hander.
 * 
 * @param c : the received char
 * @see interrupts.c.
 */
void modbus_char_recvd(uint8_t c);

/**
 * Modbus timer finished. This function is called by the interrupt handler 
 * when a MODBUS frame is finished. It must analyse the received packet 
 * and reply if required.
 * 
 * @see interrupts.c
 */
void modbus_timer(void);

#endif /* MODBUS_H */
