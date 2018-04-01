
#ifndef __MONNIKPCIDRIVER_H
#define __MONNIKPCIDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <vxWorks.h>
#include "MonNik_api.h" 


/******************************************RFM2G Structures***************************************************/

/* Static Device Configuration Information */

typedef struct  MonNikConfigVxWorks
{
	MONNIK_UINT16   Size;               /* Size of structure                   */
	MONNIK_UINT8    Unit;               /* Device unit number                  */
	MONNIK_UINT8    Device[128];        /* Name of device file                 */
	MONNIKPCICONFIG PCI;                /* PCI Bus Specific Info               */
}  MONNIKCONFIGVXWORKS;


/*****************************************************************************/



/* This structure contains all information about an MonNik device */
typedef struct MONNIKDEVICEINFOVxWorks
{
	MONNIK_UINT32        Flags;
	atomic_t            Instance;/*rfm2gOpen increments, rfm2gClose decrements*/
	MONNIKCONFIGVXWORKS *   Config;
	MONNIK_UINT8         unit; /*Minor device number, used as index in rfm2gDeviceInfo*/
	VXB_DEVICE_ID       devId;

} MONNIKDEVICEINFO;
//*PMONNIKDEVICEINFO;

/* Flags */
#define MONNIK_PRESENT     (1<<0)  /* Device is present                */
#define MONNIK_OPEN        (1<<1)  /* Device is opened                 */
#define MONNIK_TIMEDOUT    (1<<2)  /* Timed out waiting on an interupt */
#define MONNIK_DMA_ON      (1<<3)  /* DMA is turned on                 */
#define MONNIK_DMA_DONE    (1<<4)  /* DMA transfer is complete         */
#define MONNIK_IN_USE      (1<<5)  /* Sync for read/write              */





/* BIT MASK Definitions */
#define MONNIK_BIT0  0x00000001
#define MONNIK_BIT1  0x00000002
#define MONNIK_BIT2  0x00000004
#define MONNIK_BIT3  0x00000008
#define MONNIK_BIT4  0x00000010
#define MONNIK_BIT5  0x00000020
#define MONNIK_BIT6  0x00000040
#define MONNIK_BIT7  0x00000080
#define MONNIK_BIT8  0x00000100
#define MONNIK_BIT9  0x00000200
#define MONNIK_BIT10 0x00000400
#define MONNIK_BIT11 0x00000800
#define MONNIK_BIT12 0x00001000
#define MONNIK_BIT13 0x00002000
#define MONNIK_BIT14 0x00004000
#define MONNIK_BIT15 0x00008000
#define MONNIK_BIT16 0x00010000
#define MONNIK_BIT17 0x00020000
#define MONNIK_BIT18 0x00040000
#define MONNIK_BIT19 0x00080000
#define MONNIK_BIT20 0x00100000
#define MONNIK_BIT21 0x00200000
#define MONNIK_BIT22 0x00400000
#define MONNIK_BIT23 0x00800000
#define MONNIK_BIT24 0x01000000
#define MONNIK_BIT25 0x02000000
#define MONNIK_BIT26 0x04000000
#define MONNIK_BIT27 0x08000000
#define MONNIK_BIT28 0x10000000
#define MONNIK_BIT29 0x20000000
#define MONNIK_BIT30 0x40000000
#define MONNIK_BIT31 0x80000000


/* MonNik Control and Status register offset values */

#ifdef __cplusplus
}
#endif

#endif 
