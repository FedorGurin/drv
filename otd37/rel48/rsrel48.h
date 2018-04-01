#ifndef _RSREL48_H
#define _RSREL48_H

#include "rsrel48_defs.h"

struct rsrel48ReadCmd {
    unsigned startChannel;  /* channel from which to start reading */
    unsigned channelCount;  /* how much channels to read */
    //int asBitField;         /* whether buffer should be interpreted as bit field or as array of bytes */
    uint8_t values[REL48_CHANNELS_PER_BOARD * REL48_MAX_ADAPTER];  /* buffer, where to store data */
};

#endif /* _RSREL48_H */
