#ifndef _ADC32_H
#define _ADC32_H

#include "adc32Defs.h"
#include "adc32Ioctl.h"

/*
 * Here, read/write command are defined.
 */
struct adc32ReadCmd {
    int firstChannel;
    int channelCount;
    unsigned short *buffer;
};

#endif /* _ADC32_H */
