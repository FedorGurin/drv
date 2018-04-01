/*
===============================================================================
							COPYRIGHT NOTICE

	Copyright (C) 2001, 2006, 2008, GE Fanuc Intelligent Platforms, Inc.
	International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

$Workfile: rfm2g_api.h $
$Revision: 43 $
$Modtime: 7/24/08 9:24a $

-------------------------------------------------------------------------------
	Description: MONNIK API Library Function Definitions
-------------------------------------------------------------------------------

==============================================================================*/

#ifndef __MONNIK_API_H
#define __MONNIK_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*

  The rfm2g_osspec.h header file should

  1) define STDMONNIKCALL which is used for functions that return MONNIK_STATUS
  2) define MONNIKHANDLE
  3) prototypes for driver specific functions
  4) include other header files as necessary.
  5) include common API definitions header file rfm2g_defs.h
  6) Include header file that defines MONNIK types, rfm2g_types.h

 */
#include "MonNik_osspec.h"

/*****************************************************************************/
/*
	This header file defines the common PCI MONNIK API function call prototypes.
*/

/*****************************************************************************/

/* File Open/Close */

STDMONNIKCALL MONNIKOpen(char *DevicePath, MONNIKHANDLE *rh);
STDMONNIKCALL MONNIKClose(MONNIKHANDLE *rh);


/*****************************************************************************/

/* Configuration */

STDMONNIKCALL MONNIKGetConfig(MONNIKHANDLE rh, MONNIKCONFIG *Config);

STDMONNIKCALL MONNIKUserMemory(MONNIKHANDLE rh, volatile void **UserMemoryPtr,
	MONNIK_UINT64 Offset, MONNIK_UINT32 Pages);


STDMONNIKCALL MONNIKUnMapUserMemory(MONNIKHANDLE rh, volatile void **UserMemoryPtr,
	MONNIK_UINT32 Pages);



STDMONNIKCALL MONNIKSize(MONNIKHANDLE rh, MONNIK_UINT32 *SizePtr);


STDMONNIKCALL MONNIKDeviceName(MONNIKHANDLE rh, char *NamePtr);


STDMONNIKCALL MONNIKDriverVersion(MONNIKHANDLE rh, char *VersionPtr);


STDMONNIKCALL MONNIKPeek32(MONNIKHANDLE rh, MONNIK_UINT32 Offset,
						 MONNIK_UINT32 *Value);


STDMONNIKCALL MONNIKPoke32(MONNIKHANDLE rh, MONNIK_UINT32 Offset,
						 MONNIK_UINT32 Value);



STDMONNIKCALL MONNIKPoke32(MONNIKHANDLE rh, MONNIK_UINT32 Offset,
						 MONNIK_UINT32 Value);

STDMONNIKCALL MONNIKMapDeviceRegistr(MONNIKHANDLE rh,
						 unsigned int    *DeviceBaseRegistr);



STDMONNIKCALL MONNIKMapDeviceRegistr(MONNIKHANDLE rh,

						 unsigned int    *DeviceBaseRegistr);

STDMONNIKCALL MONNIKSetAdrInterrupFuction(MONNIKHANDLE rh,

						 unsigned int  AdrFunctin);


/*****************************************************************************/


/* Utility  MonNikSetAdrInterrupFuction  unsigned int  AdrFunctin */


#if defined(_WINDOWS)
DECL_SPEC(char *)  MONNIKErrorMsg(MONNIK_STATUS ErrorCode);
#else
char * MONNIKErrorMsg(MONNIK_STATUS ErrorCode);
#endif

/*****************************************************************************/

/* NOTE: Application programs should not use the following functions unless
		 directed to do so by GE Fanuc Intelligent Platforms, Inc. support
		 personnel. */

STDMONNIKCALL MONNIKGetDebugFlags(MONNIKHANDLE rh, MONNIK_UINT32 *Flags);

STDMONNIKCALL MONNIKSetDebugFlags(MONNIKHANDLE rh, MONNIK_UINT32 Flags);




#ifdef __cplusplus
}
#endif

#endif  /* __MONNIK_API_H */
