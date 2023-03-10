/************************************************************************//**
 * \file	measure.h
 * \brief	Measuring current and voltage from the solar panel
 ***************************************************************************/

#ifndef MEASURE_H
#define	MEASURE_H

#include <stdint.h>

/**
 * Initialize ADC and pins for measuring current and voltage
 */
void adc_init(void);

/**
 * Measure the voltage on the channel for the current. 
 * This function is expected to do some filtering.
 * 
 * @return The offset 
 */

uint16_t measure_voltage(void);

/**
 * Measure the current on the solar panel.
 * This function is expected to do some filtering
 * 
 * @param offset : the offset as measured by measure_current_offset 
 * @return the current in microamperes
 */

uint16_t measure_current(uint16_t offset);

#endif /* MEASURE_H */
