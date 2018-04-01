
#ifndef ARK1620_DEFS_H
#define ARK1620_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <vxWorks.h>
//! максимальное кол-во адаптеров АРК1620
#define ARK1620_MAX_DEVICES 4
#define ARK1620_MAX_CHANNEL_IN  16 
#define ARK1620_MAX_CHANNEL_OUT 20 

typedef struct TARK1620PCICCONFIG
{
	 UINT8     bus;      /* PCI bus number                        */
	 UINT8     func;     /* Function                              */
	 UINT16    type;     /* From Device ID register               */
	 UINT32    devfn;    /* Encoded device and function numbers   */
     UINT8     intNum;   /* Which interrupt*/
     UINT32    baseAddr; /* Physical base addr of board map registers */
} TARK1620PCICCONFIG;

/*typedef struct MONNIKCONFIG_
{
    UINT8    Unit;

    char     Device[128];
    char     Name[32];
    char     DriverVersion[16];
    UINT8    RevisionId;
    TAR16T16RPCICCONFIG PciConfig;
} MONNIKCONFIG;
*/
#ifdef __cplusplus
}
#endif

#endif
