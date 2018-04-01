#ifndef ARK1620PCIDRIVER_H
#define ARK1620PCIDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <vxWorks.h>

//#include "Ar16t16r_api.h"
#include "Ark1620_defs.h"

typedef struct  TARK1620CONFIG_
{
    UINT16   size;               /* Size of structure                   */
    UINT8    unit;               /* Device unit number                  */
    UINT8    device[128];        /* Name of device file                 */
    TARK1620PCICCONFIG pci;         /* PCI Bus Specific Info               */
}TARK1620CONFIG;

typedef struct TARK1620DEVICEINFO_
{
    UINT32                  flags;
    atomic_t                instance;
    TARK1620CONFIG*        config;
    UINT8                   unit;
    VXB_DEVICE_ID           devId;
}TARK1620DEVICEINFO;

#ifdef __cplusplus
}
#endif

#endif  /* __GEFRFM2GPCIDRIVER_H */
