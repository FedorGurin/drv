#ifndef _DAC16_IOS_H
#define _DAC16_IOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dac16_defs.h"
#include "dac16_ioctl.h"

typedef struct _dac16_write_cmd {
    unsigned channel[DAC16_MAX_ADAPTERS * DAC16_CHANNELS_PER_BOARD];   /* номер канала */
    float Vcode[DAC16_MAX_ADAPTERS * DAC16_CHANNELS_PER_BOARD];        /* код напряжения */
}dac16_write_cmd;
    
#ifdef __cplusplus
}
#endif

#endif /* _DACWAV_IOS_H */
