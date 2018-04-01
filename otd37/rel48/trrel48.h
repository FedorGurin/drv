#ifndef _TRREL48_IOS_H
#define _TRREL48_IOS_H

#include "trrel48_defs.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
struct trrel48WriteCmd {
    unsigned startChannel;
    unsigned channelCount;
    uint8_t values[REL48_CHANNELS_PER_BOARD * REL48_MAX_ADAPTER];
};



//void trrel48IosInit(void);
//#ifdef __cplusplus
//}
//#endif
#endif /* _TRREL48_IOS_H */
