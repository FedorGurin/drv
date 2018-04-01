#ifndef __REL48_DEFS_H
#define __REL48_DEFS_H


#include "rel48_errno.h"
#include <ioctl.h>
#define REL48_BADDR_SEP ","
#define REL48_CHANNELS_PER_BOARD 48
#define REL48_MAX_ADAPTER 8
#define REL48_WORD 16
#define REL48_REGS 3
#define REL48_IO_ADD_BASE_ADR _IOW(0xeb,  200, int)

#endif /* __REL48_DEFS_H */ 
