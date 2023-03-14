#include <xc.h>

#include "measure.h"
#include "mcc_generated_files/mcc.h"
#define VOLTAGE_CHANNEL 0
#define CURRENT_CHANNEL 1

#define ADC_RESOLUTION  (1024 - 1)
#define ADC_REFH        3300
#define GAIN            66
#define RESISTOR        3

// Number of samples to do the averaging during measures
#define AVERAGE_SAMPLES 32

void adc_init(void)
{
	// TODO -> complete adc initialisation

}

/**
 * Read one ADC channel. This function is only
 * local to this file.
 *
 * @param channel : the channel to be measured
 * @return the ADC read value
 */
static uint16_t measure_adc(uint8_t channel)
{
    uint32_t result = 0;
   for(uint8_t i=0;i<AVERAGE_SAMPLES;i++)
   {
    result += ADC_GetConversion(channel);
   }
       result = result / AVERAGE_SAMPLES;
       return result;
}

uint16_t measure_voltage()
{
   uint32_t result=measure_adc(voltage); 
   uint32_t mVolt;

   mVolt = (ADC_REFH * result) / ADC_RESOLUTION;
    return mVolt;
}

uint16_t measure_current(uint16_t offset)
{
    uint32_t result=measure_adc(current); 
    uint32_t uAmp;
    uAmp = ( result*ADC_REFH) / (ADC_RESOLUTION);
    uAmp = (uAmp*1000)/(GAIN*RESISTOR);
    
    if (uAmp<offset)
    {
        uAmp=0;
    }
        else
        {
        uAmp-=offset;
        }
      
    return uAmp;
}