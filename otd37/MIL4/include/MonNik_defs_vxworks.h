/*
===============================================================================
							COPYRIGHT NOTICE

-------------------------------------------------------------------------------

$Workfile: rfm2g_defs_vxworks.h $
$Revision: 14 $
$Modtime: 2/20/08 12:11p $

-------------------------------------------------------------------------------
	Description: RFM2g VxWorks OS Specific Header File
-------------------------------------------------------------------------------

==============================================================================*/

/*****************************************************************************/
/*

 The rfm2g_defs_vxworks.h header file should


INCLUDE FILES:

Include Dependencies

rfm2g_osspec.h         rfm2g_struct.h (Driver Side)
	|						|
	-------------------------
				|
				|
		rfm2g_defs_vxworks.h
				|
				|
			rfm2g_defs.h (Common)
				|
				|
	-------------------------
	|						|
rfm2g_types.h			rfm2g_errno.h (Common)

*/

#ifndef MONNIK_DEFS_VXWORKS_H
#define MONNIK_DEFS_VXWORKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "monnik_defs.h"

/*****************************************************************************/

/* User configurable option */
#define MONNIK_MAX_DEVICES 4



typedef struct MONNIKVXWORKSREGINFO
{
	MONNIKREGSETTYPE regset;
	MONNIK_UINT32 Offset;
	MONNIK_UINT32 Width;
	MONNIK_UINT32 Value;

} MONNIKVXWORKSREGINFO;

/*****************************************************************************/


/* MMap info */
typedef struct MonNikMmapInfo
{
	void **UserMemoryPtr;
	MONNIK_UINT32 Offset;
	MONNIK_UINT32 Pages;
	MONNIK_UINT32 Size;
}MONNIKMMAPINFO, *MONNIKMMAPINFO_P;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RFM2G_DEFS_VXWORKS_H */
