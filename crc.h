/************************************************************************//**
 * \file	crc.h
 * \brief	CRC calculation for Modbus
 ***************************************************************************/

#ifndef CRC_H
#define	CRC_H

#include <stdint.h>

/**
 * Compute the CRC using the MODBUS formulae
 * @param msg : message to be sent
 * @param length : length of the message
 * @return the computed CRC value
 */
uint16_t CRC16(const uint8_t *msg, uint16_t length);

#endif /* CRC_H */