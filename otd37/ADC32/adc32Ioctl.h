#ifndef _ADC32_IOCTL_H
#define _ADC32_IOCTL_H

#include <ioctl.h>

#define ADC32_MAGIC 0x7e

/*
 * Here IO control (ioctl) operation codes and their arguments are definied.
 */

struct adc32SetPeriodCmd {
    int period;
};

struct adc32GetPeriodCmd {
    int *buffer;
    
};

#define IOCADCSETPERIOD _IOR(ADC32_MAGIC, 0, sizeof(struct adc32SetPeriodCmd))
#define IOCADCGETPERIOD _IOW(ADC32_MAGIC, 1, sizeof(struct adc32GetPeriodCmd))
#define IOCADCREINIT    _IO(ADC32_MAGIC, 2)

#endif /* _ADC32_IOCTL_H */
