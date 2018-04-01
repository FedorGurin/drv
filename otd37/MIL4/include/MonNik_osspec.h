/* rfm2g_osspec.h - RFM2g VxWorks OS Specific Header File */

/*
===============================================================================
							COPYRIGHT NOTICE

	Copyright (C) 2001, 2006, 2009 GE Fanuc Embedded Systems, Inc.
	International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: rfm2g_osspec.h $
$Revision:  $
$Modtime:  $
*/

/*
DESCRIPTION  This file
 1) define STDRFM2GCALL which is used for functions that return RFM2G_STATUS
 2) define RFM2GHANDLE
 3) prototypes for driver specific functions
 4) include other header files as necessary.
 5) include common API definitions header file rfm2g_defs.h
 6) Include header file that defines RFM2G types, rfm2g_types.h

INCLUDE FILES:
*/

#ifndef MONNIKAPI_VXW_H
#define MONNIKAPI_VXW_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WRS_KERNEL /* Includes for Kernel library */
#include "vxWorks.h"
#else	/* Includes for RTP library */
#include <pthread.h>
#include <semaphore.h>
#endif

#include "MONNIK_types.h"
#include "MONNIK_defs.h"
#include "MONNIK_regs.h"
#include "MONNIK_defs_vxworks.h"


#ifdef _WRS_KERNEL /* Kernel Library related definitions */

/* Define a MACRO for the standard calling pattern */
#define STDMONNIKCALL MONNIK_STATUS

/* RFM2g Handle value created during RFM2gOpen */
#define MONNIKHANDLE void*

#else	/* RTP Library related definitions */

/* Define a MACRO for the standard calling pattern */
#define STDRFM2GCALL _LibDeclSpec RFM2G_STATUS _CallDeclSpec

/* RFM2GEVENT_FIFO_AF has been deprecrated and replaced with RFM2GEVENT_RXFIFO_AFULL */
#define RFM2GEVENT_FIFO_AF RFM2GEVENT_RXFIFO_AFULL

typedef struct MonNikhandle_s *MONNIKHANDLE;


/* Define the device handle */
typedef struct MonNikhandle_s
{
	MONNIK_INT32  fd;          /* File descriptor returned by open(2) call */
	MONNIKCONFIG  Config;      /* Information about opened RFM device      */
	MONNIK_UINT32 struct_id;
} MonNikhandle_t;

#endif /* _WRS_KERNEL */

#ifdef __cplusplus
}
#endif

#endif
