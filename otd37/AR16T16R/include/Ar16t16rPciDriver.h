#ifndef AR16T16RPCIDRIVER_H
#define AR16T16RPCIDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <vxWorks.h>

#include "Ar16t16rApi.h"
#include "Ar16t16r_defs.h"

typedef struct  TAR16T16RCONFIG_
{
    uint16_t   size;               /* Size of structure                   */
    uint8_t    unit;               /* Device unit number                  */
    uint8_t    device[128];        /* Name of device file                 */
    TAR16T16RPCICCONFIG pci;         /* PCI Bus Specific Info               */
}TAR16T16RCONFIG;

typedef struct TAR16T16RDEVICEINFO_
{
    uint32_t                flags;
    atomic_t                instance;
    TAR16T16RCONFIG*        config;
    uint8_t                 unit;
    VXB_DEVICE_ID           devId;
}TAR16T16RDEVICEINFO;

#ifdef __cplusplus
}
#endif

#endif  
