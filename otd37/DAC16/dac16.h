#ifndef _DAC16_IOS_H
#define _DAC16_IOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dac16_defs.h"
#include "dac16_ioctl.h"

typedef struct _dac16_write_cmd {
    unsigned channel;   /* channel number (0-15) */
    float Vs;           /* current voltage */    
}dac16_write_cmd;
    
#ifdef __cplusplus
}
#endif

#endif /* _DACWAV_IOS_H */
