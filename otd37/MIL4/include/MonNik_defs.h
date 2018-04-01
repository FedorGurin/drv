/*
===============================================================================
                            COPYRIGHT NOTICE


-------------------------------------------------------------------------------

$Workfile: rfm2g_defs.h $
$Revision: 33 $
$Modtime: 1/22/09 3:15p $

-------------------------------------------------------------------------------
	Description: RFM2G Common API Library Definitions
-------------------------------------------------------------------------------


==============================================================================*/

#ifndef __MONIK_defs_H
#define  __MONIK_defs_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/* Standard defines */
#include "monnik_types.h"
#include "monnik_errno.h"

/*
 * This header file should
 *
 * 1) Define definitions for use with MONNIK_BOOL
 * 2) Define definitions for use with MONNIK_NODE
 * 3) Define RFM2GCONFIG
 * 4) Define RFM2GEVENTTYPE enumeration type
 * 5) Define RFM2GEVENTINFO structure type
 * 6) Define driver debug message class definitions
 */

/*****************************************************************************/

/* Definitions for use with MONNIK_BOOL */

#define MONNIK_ENABLE  MONNIK_TRUE
#define MONNIK_DISABLE MONNIK_FALSE

#define MONNIK_ON      MONNIK_TRUE
#define MONNIK_OFF     MONNIK_FALSE

/*****************************************************************************/


/*****************************************************************************/

/* MONNIKCONFIG structure is used by the MONNIKGetConfig function.  */

typedef struct monnikPciConfig
{
	MONNIK_UINT8     bus;            /* PCI bus number                        */
	MONNIK_UINT8     function;       /* Function                              */
	MONNIK_UINT16    type;           /* From Device ID register               */
	MONNIK_UINT32    devfn;          /* Encoded device and function numbers   */
	MONNIK_UINT32    MonNikBaseReg;      /* Physical base addr of board registers */
	MONNIK_UINT32    MonNikWindowSize;/* Size of PCI Memory window             */
	MONNIK_UINT8    *pBaseAddress;      /* Mapped base address of RFM board    */
	MONNIK_UINT8     interruptNumber;  /* Which interrupt ? */
	MONNIK_UINT32   InterruptAdr;        /*     
		                                  */
	MONNIK_UINT32    BaseAddress;      /* Physical base addr of board map registers */

} MONNIKPCICONFIG;

/*****************************************************************************/

/* RFM2GCONFIG structure is used by the RFM2gGetConfig function.  */

typedef struct MONNIKCONFIG_
{
	MONNIK_UINT8    Unit;               /* Device unit number                 */

	char           Device[128];        /* Name of device file                */
	char           Name[32];/* Product ordering option "VMISFT-RFM2G-ABC-DEF"*/
	char           DriverVersion[16];  /* Release level of driver            */
	MONNIK_UINT8    RevisionId;         /* PCI Revision ID                    */
	MONNIKPCICONFIG PciConfig;          /* PCI Bus Specific Info              */
} MONNIKCONFIG;

/*****************************************************************************/

/* Interrupt Events */

/*****************************************************************************/

/* Driver debug message class definitions */

#define MONNIK_DBERROR         (1<<0)   /* Report critical errors             */
#define MONNIK_DBINIT          (1<<1)   /* Trace device probing and search    */
#define MONNIK_DBINTR          (1<<2)   /* Trace interrupt service            */
#define MONNIK_DBIOCTL         (1<<3)   /* Trace ioctl(2) system calls        */
#define MONNIK_DBMMAP          (1<<4)   /* Trace mmap(2) system calls         */
#define MONNIK_DBOPEN          (1<<5)   /* Trace open(2) system calls         */
#define MONNIK_DBCLOSE         (1<<6)   /* Trace close(2) system calls        */
#define MONNIK_DBREAD          (1<<7)   /* Trace read(2) system calls         */
#define MONNIK_DBWRITE         (1<<8)   /* Trace write(2) system calls        */
#define MONNIK_DBPEEK          (1<<9)   /* Trace peeks                        */
#define MONNIK_DBPOKE          (1<<10)  /* Trace pokes                        */
#define MONNIK_DBSTRAT         (1<<11)  /* Trace read/write strategy          */
#define MONNIK_DBTIMER         (1<<12)  /* Trace interval timer               */
#define MONNIK_DBTRACE         (1<<13)  /* Trace subroutine entry/exit        */
#define MONNIK_DBMUTEX         (1<<14)  /* Trace synchronization and locking  */
#define MONNIK_DBINTR_NOT      (1<<15)  /* Trace non-RFM interrupt service    */
#define MONNIK_DBSLOW          (1<<16)  /* Let syslogd get the message        */
#define MONNIK_DBMINPHYS       (1<<17)  /* Trace minphys limits               */
#define MONNIK_DBDMA			  (1<<18)  /* Trace DMA Calls					 */

/*****************************************************************************/

/* MONNIKREGSETTYPE is used by MONNIKRegWrite and MONNIKRegRead functions  */

typedef enum MONNIKRegSetType
{
   EMPTY  
   /* BAR 0 mem - Local Config, Runtime and DMA Registers  */
}  MONNIKREGSETTYPE;

/*****************************************************************************/



#ifdef __cplusplus
}
#endif

#endif
