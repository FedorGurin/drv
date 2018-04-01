#ifndef _DACWAV_IOS_H
#define _DACWAV_IOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dacwav_defs.h"
#include "dacwav_ioctl.h"

typedef struct _dacwav_write_cmd {
    unsigned channel;   /* channel number (0-15) */
    float Vs;           /* starting voltage */
    float dV;           /* delta voltage */
    unsigned time;      /* time in which we should get Vs+dV on channel (us) */
}dacwav_write_cmd;
    
#ifdef __cplusplus
}
#endif

#endif /* _DACWAV_IOS_H */
