/* rfm2g_types.h - RFM2g VxWorks RFM2g Type definition file */

/*
===============================================================================
							COPYRIGHT NOTICE

	Copyright (C) 2001, 2006, 2009 GE Fanuc Embedded Systems, Inc.
	International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: rfm2g_types.h $
$Revision: 10 $
$Modtime: 7/24/08 1:54p $
*/

/*
DESCRIPTION


INCLUDE FILES:
*/

#ifndef MONNIKTYPES_VXW_H
#define MONNIKTYPES_VXW_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WRS_KERNEL	/* Defines for Kernel Library */

#include "vxWorks.h"

/*--------------------------------------------------------
 * Define MomNik INT Values
 --------------------------------------------------------*/
#ifndef MONNIK_INT8
#define MONNIK_INT8 INT8
#endif
#ifndef MONNIK_INT16
#define MONNIK_INT16 INT16
#endif
#ifndef MONNIK_INT32
#define MONNIK_INT32 INT32
#endif
#ifndef MONNIK_INT64
#define MONNIK_INT64 INT64
#endif
#ifndef MONNIK_UINT8
#define MONNIK_UINT8 UINT8
#endif
#ifndef MONNIK_UINT16
#define MONNIK_UINT16 UINT16
#endif
#ifndef MONNIK_UINT32
#define MONNIK_UINT32 UINT32
#endif
#ifndef MONNIK_UINT64
#define MONNIK_UINT64 unsigned long long
#endif

#ifndef MONNIK_BOOL
# define MONNIK_BOOL MONNIK_UINT8
#endif

#ifndef MONNIK_NODE
# define MONNIK_NODE MONNIK_UINT16
#endif

#ifndef MONNIK_CHAR
#define MONNIK_CHAR char
#endif

#ifndef MONNIK_ADDR
#define MONNIK_ADDR MONNIK_UINT32
#endif

/* Add base constants for the Boolean */
#define MONNIK_TRUE   1
#define MONNIK_FALSE  0


#else	/* Defines for RTP Library (POSIX standards) */

/* Provide Type Definitions for basic portable types */
typedef unsigned char           MONNIK_UINT8;
typedef unsigned short int      MONNIK_UINT16;
typedef unsigned int            MONNIK_UINT32;
typedef unsigned long long int  MONNIK_UINT64;

typedef signed char             MONNIK_INT8;
typedef signed short int        MONNIK_INT16;
typedef signed int              MONNIK_INT32;
typedef signed long long int    MONNIK_INT64;

typedef unsigned char           MONNIK_BOOL;

typedef char                    MONNIK_CHAR;
typedef unsigned long           MONNIK_ADDR;

/* We also use void * and char *, but these are
   C standard types, and do not need to be declares */


/*****************************************************************************/

/* Add base constants for the Boolean */
#define MONNIK_TRUE  ((MONNIK_BOOL) 1)
#define MONNIK_FALSE ((MONNIK_BOOL) 0)


/*****************************************************************************/

/* Define the scalar types */


/*****************************************************************************/

/* Define symbols to specify data width */
typedef enum MONNIKDataWidth
{
  MONNIK_BYTE      = 1,
  MONNIK_WORD      = 2,
  MONNIK_LONG      = 4,
  MONNIK_LONGLONG  = 8
} MONNIKDATAWIDTH;


/*****************************************************************************/

/* If needed, define the Maximum Path */
#define MAX_PATH 128


/*****************************************************************************/

/* Compiler support #defines */
#ifdef    __cplusplus
#define _LibDeclSpec extern "C"
#else
#define _LibDeclSpec
#endif    /* __cplusplus */

#define _CallDeclSpec

#endif /* _WRS_KERNEL */

#ifdef __cplusplus
}
#endif

#endif
