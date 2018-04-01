#ifndef _ADC32_DEFS_H
#define _ADC32_DEFS_H

#include "adc32Errno.h"

#define ADC32_CHANNELS_PER_BOARD 32
#define ADC32_DEVPATH "/adc32/"
#define ADC32_NAME "adc32"

#define ADC32_PORT_OFFSET(base, offset) base + offset
#define ADC32_DATA_PORT(base)         ADC32_PORT_OFFSET(base, 0x0)
#define ADC32_CHANNEL_PORT(base)      ADC32_PORT_OFFSET(base, 0x2)
#define ADC32_PERIOD_PORT(base)       ADC32_PORT_OFFSET(base, 0x4)
#define ADC32_ENABLE_PORT(base)       ADC32_PORT_OFFSET(base, 0x6)
#define ADC32_INIT_PORT(base)         ADC32_PORT_OFFSET(base, 0x8)
#define ADC32_CONFIG_PORT(base)       ADC32_PORT_OFFSET(base, 0xA)

#define ADC32_DEFAULT_PERIOD 0x0

/*extern int adc32GetConfig(char **);*/

#endif /* _ADC32_DEFS_H */
