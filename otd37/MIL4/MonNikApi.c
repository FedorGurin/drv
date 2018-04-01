/* MonNikApi.c - Kernel API Library source code for GE Fanuc MONNIK device */

/*
===============================================================================
							COPYRIGHT NOTICE

	Copyright (C) 2001, 2006, 2009 GE Fanuc Embedded Systems, Inc.
	International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: MonNikApi.c $
$Revision: $
$Modtime: $
*/

/*
DESCRIPTION


INCLUDE FILES:
*/

#include <vxWorks.h>
#include <intLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <vxBusLib.h>
#include <hwif/util/hwMemLib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPlbLib.h>
#include <hwif/vxbus/hwConf.h>
#include <hwif/util/vxbParamSys.h>
#include  "include/MonNik_api.h"
/* debug */
#ifndef MONNIK_LIB_DBG

#   define MONNIK_LIB_DBG(level,fmt,a,b,c,d,e,f) \
				   if ( MonNikLibDebugLevel & level ) \
					   logMsg(fmt,a,b,c,d,e,f)
#endif /* GEF_MONNIK_LIB_DBG */

#define MONNIK_KERNEL_API_VERSION "R01.00"




DEVMETHOD_DEF(ikiranMonNikDevice, "Nikolaenko Monitor Device");

DEVMETHOD_DEF(ikiranMonNikOpen, "Nikolaenko Monitor Open");
DEVMETHOD_DEF(ikiranMonNikClose, "Nikolaenko Monitor Close");
DEVMETHOD_DEF(ikiranMonNikGetConfig, "Nikolaenko Monitor Get Config");
DEVMETHOD_DEF(ikiranMonNikMapUserMemory, "Nikolaenko Monitor Map User Memory");
DEVMETHOD_DEF(ikiranMonNikUnMapUserMemory, "Nikolaenko Monitor Unmap User Memory");

DEVMETHOD_DEF(ikiranMonNikSize, "Nikolaenko Monitor Size");
DEVMETHOD_DEF(ikiranMonNikDeviceName, "Nikolaenko Monitor Device Name");
DEVMETHOD_DEF(ikiranMonNikDriverVersion, "Nikolaenko Monitor Driver Version");

DEVMETHOD_DEF(ikiranMonNikPeek32, "Nikolaenko Monitor Peek32");
DEVMETHOD_DEF(ikiranMonNikPoke32, "Nikolaenko Monitor Poke32");


DEVMETHOD_DEF(ikiranMonNikGetDebugFlags, "Nikolaenko Monitor Get Debug Flags");
DEVMETHOD_DEF(ikiranMonNikSetDebugFlags, "Nikolaenko Monitor Set Debug Flags");

DEVMETHOD_DEF(ikiranMonNikMapDeviceRegistr, "Nikolaenko Monitor MapDeviceRegistr");
DEVMETHOD_DEF(ikiranMonNikSetAdrInterrupFuction, "Nikolaenko Monitor SetAdrInterrupFuction");


typedef enum
{
	MONNIK_BUS_PCI = 0,
	MONNIK_BUS_NONE = 2
}MONNIK_TYPE;

typedef struct
{
	FUNCPTR open;
	FUNCPTR close;
	FUNCPTR getConfig;
	FUNCPTR mapUserMem;
	FUNCPTR unmapUserMem;
	FUNCPTR size;
	FUNCPTR devName;
	FUNCPTR drvVersion;
	FUNCPTR peek32;
	FUNCPTR poke32;
	FUNCPTR getDebugFlags;
	FUNCPTR setDebugFlags;
	FUNCPTR MapDeviceRegistr;
	FUNCPTR SetAdrInterrupFuction;
	VXB_DEVICE_ID devID;
}MONNIK_DRIVER_INSTANCE;

typedef struct
{
	UINT32                    devNo;
	MONNIK_DRIVER_INSTANCE *   drvInst;
	MONNIK_UINT32              structId;
}MonNikhandle_t;

/* Default DMA threshold */
#define MONNIK_DEFAULT_DMA_THRESHOLD 32
#define MONNIK_STRUCT_ID (MONNIK_UINT32)(('R'<< 24) | ('F'<<16) | ('M'<<8) | ('2'))

LOCAL MONNIK_DRIVER_INSTANCE MonNikDevice[MONNIK_MAX_DEVICES];

LOCAL BOOL MonNikLibInitDone = FALSE;

LOCAL int MonNikPciCount = 0;

LOCAL UINT32 MonNikLibDebugLevel=0;





STATUS MonNikPciDevCount(MONNIK_UINT32 *count)
{
	if (count == NULL)
	{
		return ERROR;
	}
	*count = MonNikPciCount;
	return OK;
}

LOCAL STATUS MonNikDeviceHelper ( VXB_DEVICE_ID devID, void * dummy)
 {
	FUNCPTR	fptrDevice;
/*BOOL alarmSupported = FALSE; */
	MONNIK_TYPE rfmType = MONNIK_BUS_NONE;
	int rfmCnt = 0;

	/* check for the validity of the function parameters */
	if (devID == NULL)
	{
		return (ERROR);
	}

	fptrDevice = vxbDevMethodGet (devID, DEVMETHOD_CALL(ikiranMonNikDevice));
	if (fptrDevice == NULL)
	{
		/* This is not an RFM device */
		return ERROR;
	}
	/* Found RFM */
	/* Probe for type of RFM device */
	(*fptrDevice)(devID, &rfmType);
	if(rfmType == MONNIK_BUS_PCI)
	{
		rfmCnt = MonNikPciCount;
		MonNikPciCount++;
	}


	if(rfmCnt > MONNIK_MAX_DEVICES)
	{
		MONNIK_LIB_DBG(MONNIK_DBERROR,"MonNikDeviceHelper : MONNIK Device support limited to %d devices\n",
						  MONNIK_MAX_DEVICES,0,0,0,0,0);
		return ERROR;

	}

	/* only store valid sensors */
	MonNikDevice[rfmCnt].open = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikOpen));
	MonNikDevice[rfmCnt].close =	vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikClose));
	MonNikDevice[rfmCnt].getConfig = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikGetConfig));
	MonNikDevice[rfmCnt].mapUserMem = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikMapUserMemory));
	MonNikDevice[rfmCnt].unmapUserMem = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikUnMapUserMemory));

	MonNikDevice[rfmCnt].size = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikSize));

	MonNikDevice[rfmCnt].devName = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikDeviceName));
	MonNikDevice[rfmCnt].drvVersion = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikDriverVersion));
	MonNikDevice[rfmCnt].peek32 = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikPeek32));
	MonNikDevice[rfmCnt].poke32 = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikPoke32));
	MonNikDevice[rfmCnt].getDebugFlags = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikGetDebugFlags));
	MonNikDevice[rfmCnt].setDebugFlags = vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikSetDebugFlags));

	MonNikDevice[rfmCnt].MapDeviceRegistr=vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikMapDeviceRegistr));

	MonNikDevice[rfmCnt].SetAdrInterrupFuction=vxbDevMethodGet(devID, DEVMETHOD_CALL(ikiranMonNikSetAdrInterrupFuction));


	MonNikDevice[rfmCnt].devID = devID;

	return OK;
}




STATUS MonNikLibInit(void)
{
	int rfmType = 0;	/* Type of RFM devices - PCI or VME */
	int rfmCnt = 0; 	/* Number of RFM devices supported for each type */

	if(MonNikLibInitDone == TRUE)
	{
		MONNIK_LIB_DBG(MONNIK_DBSLOW,"MonNikLibInit : Lib is already initialised\n",
								 0,0,0,0,0,0);
		return OK;
	}
	for (rfmType = 0; rfmType < 2; rfmType++)
	{
		for(rfmCnt = 0; rfmCnt < MONNIK_MAX_DEVICES; rfmCnt++)
		{
			/* Initialize function pointers */
			MonNikDevice[rfmCnt].devID = NULL;
			MonNikDevice[rfmCnt].open = NULL;
			MonNikDevice[rfmCnt].close = NULL;
			MonNikDevice[rfmCnt].getConfig = NULL;
			MonNikDevice[rfmCnt].mapUserMem = NULL;
			MonNikDevice[rfmCnt].unmapUserMem = NULL;
			MonNikDevice[rfmCnt].size = NULL;
			MonNikDevice[rfmCnt].devName = NULL;
			MonNikDevice[rfmCnt].drvVersion = NULL;
			MonNikDevice[rfmCnt].peek32 = NULL;
			MonNikDevice[rfmCnt].poke32 = NULL;
			MonNikDevice[rfmCnt].getDebugFlags = NULL;
			MonNikDevice[rfmCnt].setDebugFlags = NULL;
			MonNikDevice[rfmCnt].MapDeviceRegistr= NULL;
			MonNikDevice[rfmCnt].SetAdrInterrupFuction=NULL;
		}
	}

	vxbDevIterate(MonNikDeviceHelper,NULL, VXB_ITERATE_INSTANCES);

	MonNikLibInitDone = TRUE;
	MONNIK_LIB_DBG(MONNIK_DBSLOW,"MonNikLibInit: Found %d PCI-MONNIK devices and %d VME-MONNIK devices\n",
					  MonNikPciCount, 0,0,0,0,0);

	return OK;
}

/**************************************************************************
*
* MONNIKCheckHandle - used internally by the driver to validate the handle
*
* Check that the handle used by the driver is valid
*
* RETURNS: If successful it returns MONNIK_SUCCESS, otherwise return error code.
*          See MonNik_errno.h for all the supported codes.
*/
LOCAL STATUS
MONNIKCheckHandle(MonNikhandle_t * handle)
{
	if (MonNikLibInitDone != MONNIK_TRUE)
	{
	   return(MONNIK_NOT_INITIALIZED);
	}
	if (handle == NULL)
	{
		return MONNIK_NULL_DESCRIPTOR;
	}
	if (handle->structId != MONNIK_STRUCT_ID)
	{
		return MONNIK_BAD_PARAMETER_1;
	}
	MONNIK_LIB_DBG(MONNIK_DBSLOW,"MONNIKCheckHandle: Handle passed\n",0,0,0,0,0,0);
	return OK;
}

/**************************************************************************
*
* MONNIKOpen - Open and init the MONNIK device
*
* Open and init the MONNIK device.
* devicePath must be in this format (deviceName_instance)
* ex. "MONNIK_0" or "MONNIK_1", etc...
*
* RETURNS: MONNIK_STATUS
*/

STDMONNIKCALL
MONNIKOpen(char			*devicePath,
		  MONNIKHANDLE 	*rh)
{
	char *me = "MONNIKOpen";
	char *pcDevNum    = NULL;

	MONNIK_STATUS status = ERROR;
	MONNIK_TYPE rfmType;
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle;
	int rfmNo;

	if (MonNikLibInitDone != MONNIK_TRUE)
	{
	   return(MONNIK_NOT_INITIALIZED);
	}
	if(devicePath == NULL)
	{
	   return(MONNIK_BAD_PARAMETER_1);
	}
	if(rh == NULL)
	{
	   return(MONNIK_BAD_PARAMETER_2);
	}
	if(*rh != NULL)
	{
	  return(MONNIK_HANDLE_NOT_NULL);
	}

	/*
	 * Decode the device path to determine the instance of the board to
	 * search for
	 */
	if((strncmp(devicePath, "/MONNIKPCI", 10)== 0))
	{
		MONNIK_LIB_DBG(MONNIK_DBSLOW,"%s: Requested PCI device\n",(int)me,0,0,0,0,0);
		rfmType = MONNIK_BUS_PCI;

	}
	else
	{
		return(MONNIK_BAD_PARAMETER_1);
	}

	pcDevNum = strchr(devicePath,'_');
	if(pcDevNum == NULL)
	{
		return(MONNIK_BAD_PARAMETER_1);
	}
	pcDevNum++;
	rfmNo = -1;
	rfmNo = atoi(pcDevNum);
	if ( (rfmNo < 0) || (rfmNo >= MONNIK_MAX_DEVICES) )
	{
		return(MONNIK_BAD_PARAMETER_1);
	}
	drvInst = (MONNIK_DRIVER_INSTANCE *)&MonNikDevice[rfmNo];

	MONNIK_LIB_DBG(MONNIK_DBSLOW,"%s: Requested device Number:0x%x\n",(int)me,rfmNo,0,0,0,0);


	/* initialize the handle structure for this open call */
	handle = (MonNikhandle_t *) hwMemAlloc (sizeof (MonNikhandle_t));
	if(handle == NULL)
	{
		MONNIK_LIB_DBG(MONNIK_DBERROR,"%s:Cannot allocate handle memory\n",(int)me,0,0,0,0,0);
		return( MONNIK_LOW_MEMORY );
	}
	bzero ( (char *)handle, sizeof(MonNikhandle_t));

	/* Open the device driver */
	status = (drvInst->open)(drvInst->devID);
	if(status != MONNIK_SUCCESS)
	{
		MONNIK_LIB_DBG(MONNIK_DBERROR,"%s: Driver Error\n",(int)me,0,0,0,0,0);
		return status;
	}

	handle->drvInst = drvInst;
	handle->devNo   = rfmNo;

	/* Initialize DMA Threshold */
	handle->structId        = MONNIK_STRUCT_ID;

	*rh                     = (MONNIKHANDLE)handle;

	MONNIK_LIB_DBG(MONNIK_DBSLOW,"%s: Lib open success\n",(int)me,0,0,0,0,0);
	return( MONNIK_SUCCESS );

}

/**************************************************************************
*
* MONNIKClose - Close down the handle created with MONNIKOpen
*
* Close down the handle created with MONNIKOpen
*
* RETURNS: MONNIK_STATUS
*/

STDMONNIKCALL
MONNIKClose( MONNIKHANDLE *rh )
{
	char *me = "MONNIKClose";
	MonNikhandle_t *handle;
	MONNIK_DRIVER_INSTANCE * drvInst;
	MONNIK_STATUS status;

	if (rh == NULL)
	{
		return MONNIK_BAD_PARAMETER_1;
	}
	handle = (MonNikhandle_t *)*rh;
	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}

	/* Cancel all callbacks/waitforevents belonging to this handle */
	drvInst = handle->drvInst;
	status  = (*drvInst->close)(drvInst->devID);
	if(status != MONNIK_SUCCESS)
	{
		MONNIK_LIB_DBG(MONNIK_DBERROR,"%s: Close returned error\n",(int)me,0,0,0,0,0);
	}

	/* Zero out the handle's data */
	bzero ( (char *)handle, sizeof(MonNikhandle_t));

	/* Deallocate the MONNIKHANDLE structure */
	hwMemFree( (void *)(handle) );

	*rh = (MONNIKHANDLE) NULL;

	return( MONNIK_SUCCESS );

}
/**************************************************************************
*
* MONNIKGetConfig - Get configuration information for the requested MONNIK Card
*
* Get a copy of configuration information for the requested MONNIK Card.
*
* RETURNS: MONNIK_STATUS
*/

STDMONNIKCALL
MONNIKGetConfig(MONNIKHANDLE rh, MONNIKCONFIG *Config)
{
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	MONNIK_DRIVER_INSTANCE * drvInst;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}

	/* Check the MONNIKCONFIG pointer */
	if( Config == (MONNIKCONFIG *) NULL )
	{
		return( MONNIK_BAD_PARAMETER_2 );
	}

	drvInst = handle->drvInst;
	return (*drvInst->getConfig)(drvInst->devID, Config);
}



/**************************************************************************
*
* MONNIKSize - Get ths size of the memory provided on the MONNIK Card
*
* Get ths size of the memory provided on the MONNIK Card.
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKSize(MONNIKHANDLE rh, MONNIK_UINT32 *SizePtr)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	if (SizePtr == NULL)
	{
		return (MONNIK_BAD_PARAMETER_2);
	}
	drvInst = handle->drvInst;
	return (*drvInst->size)(drvInst->devID, SizePtr);
}

/**************************************************************************
*
* MONNIKDeviceName - Get the MONNIK device name for the given handle
*
* Get the MONNIK device name for the given handle.
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKDeviceName(MONNIKHANDLE rh, char *NamePtr)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}

	if (NamePtr == NULL)
	{
		return (MONNIK_BAD_PARAMETER_2);
	}
	drvInst = handle->drvInst;
	return (*drvInst->devName)(drvInst->devID, NamePtr);
}

/**************************************************************************
*
* MONNIKDriverVersion - Get the MONNIK Driver version for the given handle
*
* Get the MONNIK Driver version for the given handle
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKDriverVersion(MONNIKHANDLE rh, char *VersionPtr)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	if (VersionPtr == NULL)
	{
		return (MONNIK_BAD_PARAMETER_2);
	}
	drvInst = handle->drvInst;
	return (*drvInst->drvVersion)(drvInst->devID, VersionPtr);
}



/**************************************************************************
*
* MONNIKPeek32 - Read 32 bits of data
*
* Read 32 bits of data.  For this routine to work correctly, it is assumed
* that data in RFM memory is little endian.
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKPeek32(MONNIKHANDLE   rh,
			MONNIK_UINT32  Offset,
			MONNIK_UINT32 *Value)
{
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	MONNIK_DRIVER_INSTANCE * drvInst;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	if(Value == NULL)
	{
		return( MONNIK_BAD_PARAMETER_3 );
	}

	drvInst = handle->drvInst;
	return (*drvInst->peek32)(drvInst->devID, Offset, Value);
}


/**************************************************************************
*
* MONNIKPoke32 - write 32 bits of data
*
* write 32 bits of data.  For this routine to work correctly, it is assumed
* that data in RFM memory is little endian.
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKPoke32(MONNIKHANDLE   rh,
			MONNIK_UINT32  Offset,
			MONNIK_UINT32  Value)
{
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	MONNIK_DRIVER_INSTANCE * drvInst;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}

	drvInst = handle->drvInst;
	return (*drvInst->poke32)(drvInst->devID, Offset, Value);
}

/**************************************************************************
*
* MONNIKUserMemory - Map a pointer to reflective memory
*
* This routine maps a user-supplied pointer into the Reflective
* Memory address space, allowing direct access to RFM memory via pointer
* dereferencing.
*
* RETURNS: MONNIK_SUCCESS.
*/
STDMONNIKCALL
MONNIKUserMemory(MONNIKHANDLE     rh,
				volatile void **UserMemoryPtr,
				MONNIK_UINT64    Offset,
				MONNIK_UINT32    Pages)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	if(UserMemoryPtr == NULL)
	{
		return(MONNIK_BAD_PARAMETER_2);
	}
	if(Pages == 0)
	{
		return(MONNIK_BAD_PARAMETER_4);
	}

	drvInst = handle->drvInst;
	return (*drvInst->mapUserMem)(drvInst->devID, UserMemoryPtr, Offset, Pages);

}


/*****************************************************************************/

/* NOTE: Application programs should not use the following functions unless
		 directed to do so by GE Fanuc Intelligent Platforms, Inc. support
		 personnel. */

/**************************************************************************
*
* MONNIKGetDebugFlags - Get the debug state of the driver
*
* Get the debug state of the driver
*              typedef enum
*              {
*                MONNIK_DEBUG_OFF = 0,
*                MONNIK_DEBUG_ON = 1
*              }DEBUG_FLAG;
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKGetDebugFlags(MONNIKHANDLE   rh,
				   MONNIK_UINT32 *Flags)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	if (Flags == NULL)
	{
		return(MONNIK_BAD_PARAMETER_2);
	}
	drvInst = handle->drvInst;
	return (*drvInst->getDebugFlags)(drvInst->devID, Flags);
}


STDMONNIKCALL MONNIKMapDeviceRegistr(MONNIKHANDLE rh,

						 unsigned int    *DeviceBaseRegistr)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}

drvInst = handle->drvInst;
return (*drvInst->MapDeviceRegistr)(drvInst->devID,DeviceBaseRegistr);
}




STDMONNIKCALL MONNIKSetAdrInterrupFuction(MONNIKHANDLE rh,

						 unsigned int  AdrFunctin)
{
MONNIK_DRIVER_INSTANCE *drvInst;
MonNikhandle_t *handle = (MonNikhandle_t *)rh;
STATUS status;

status = MONNIKCheckHandle(handle);
if (status != OK)
  {
		return( status );
  }



drvInst = handle->drvInst;
return (*drvInst->SetAdrInterrupFuction)(drvInst->devID,AdrFunctin);
}



/**************************************************************************
*
* MONNIKSetDebugFlags - Set the debug state of the driver.
*
* Set the debug state of the driver.
*              typedef enum
*              {
*                MONNIK_DEBUG_OFF = 0,
*                MONNIK_DEBUG_ON = 1
*              }DEBUG_FLAG;
*
* RETURNS: MONNIK_STATUS
*/
STDMONNIKCALL
MONNIKSetDebugFlags(MONNIKHANDLE  rh,
				   MONNIK_UINT32 Flags)
{
	MONNIK_DRIVER_INSTANCE *drvInst;
	MonNikhandle_t *handle = (MonNikhandle_t *)rh;
	STATUS status;

	status = MONNIKCheckHandle(handle);
	if (status != OK)
	{
		return( status );
	}
	drvInst = handle->drvInst;
	return (*drvInst->setDebugFlags)(drvInst->devID, Flags);

}




/* Utility */

const char *MonNik_errlist[] =
	{
	/*  0:MONNIK_SUCCESS               */  "No error",
	/*  1:MONNIK_NOT_IMPLEMENTED       */  "Capability not implemented",
	/*  2:MONNIK_DRIVER_ERROR          */  "Nonspecific error",
	/*  3:MONNIK_TIMED_OUT             */  "A wait timed out",
	/*  4:MONNIK_LOW_MEMORY            */  "A memory allocation failed",
	/*  5:MONNIK_MEM_NOT_MAPPED        */  "Memory is not mapped for this device",
	/*  6:MONNIK_OS_ERROR              */  "Function failed for other OS defined error",
	/*  7:MONNIK_EVENT_IN_USE          */  "The Event is already being waited on",
	/*  8:MONNIK_NOT_SUPPORTED         */  "Capability not supported by this particular Driver/Board",
	/*  9:MONNIK_NOT_OPEN              */  "Device not open",
	/* 10:MONNIK_NO_MONNIK_BOARD        */  "Driver did not find MONNIK device",
	/* 11:MONNIK_BAD_PARAMETER_1       */  "Function Parameter 1 in MONNIK API call is either NULL or invalid",
	/* 12:MONNIK_BAD_PARAMETER_2       */  "Function Parameter 2 in MONNIK API call is either NULL or invalid",
	/* 13:MONNIK_BAD_PARAMETER_3       */  "Function Parameter 3 in MONNIK API call is either NULL or invalid",
	/* 14:MONNIK_BAD_PARAMETER_4       */  "Function Parameter 4 in MONNIK API call is either NULL or invalid",
	/* 15:MONNIK_BAD_PARAMETER_5       */  "Function Parameter 5 in MONNIK API call is either NULL or invalid",
	/* 16:MONNIK_BAD_PARAMETER_6       */  "Function Parameter 6 in MONNIK API call is either NULL or invalid",
	/* 17:MONNIK_BAD_PARAMETER_7       */  "Function Parameter 7 in MONNIK API call is either NULL or invalid",
	/* 18:MONNIK_BAD_PARAMETER_8       */  "Function Parameter 8 in MONNIK API call is either NULL or invalid",
	/* 19:MONNIK_BAD_PARAMETER_9       */  "Function Parameter 9 in MONNIK API call is either NULL or invalid",
	/* 20:MONNIK_OUT_OF_RANGE          */  "Board offset/range exceeded",
	/* 21:MONNIK_MAP_NOT_ALLOWED       */  "Board Offset is not legal",
	/* 22:MONNIK_LINK_TEST_FAIL        */  "Link Test failed",
	/* 23:MONNIK_MEM_READ_ONLY         */  "Function attempted to change memory outside of User Memory area",
	/* 24:MONNIK_UNALIGNED_OFFSET      */  "Offset not aligned for width",
	/* 25:MONNIK_UNALIGNED_ADDRESS     */  "Address not aligned for width",
	/* 26:MONNIK_LSEEK_ERROR           */  "lseek operation failed",
	/* 27:MONNIK_READ_ERROR            */  "read operation failed",
	/* 28:MONNIK_WRITE_ERROR           */  "write operation failed",
	/* 29:MONNIK_HANDLE_NOT_NULL       */  "Cannot initialize a non-NULL handle pointer",
	/* 30:MONNIK_MODULE_NOT_LOADED     */  "Driver not loaded",
	/* 31:MONNIK_NOT_ENABLED           */  "An attempt was made to use an interrupt that has not been enabled",
	/* 32:MONNIK_ALREADY_ENABLED       */  "An attempt was made to enable an interrupt that was already enabled",
	/* 33:MONNIK_EVENT_NOT_IN_USE      */  "No process is waiting on the interrupt",
	/* 34:MONNIK_BAD_MONNIK_BOARD_ID    */  "Invalid MONNIK Board ID",
	/* 35:MONNIK_NULL_DESCRIPTOR       */  "MONNIK Handle is null",
	/* 36:MONNIK_WAIT_EVENT_CANCELLED  */  "Event Wait Cancelled",
	/* 37:MONNIK_DMA_FAILED            */  "DMA failed",
	/* 38:MONNIK_NOT_INITIALIZED       */  "User has not called MONNIKInit() yet",
	/* 39:MONNIK_UNALIGNED_LENGTH      */  "Data transfer length not 4 byte aligned",
	/* 40:MONNIK_SIGNALED              */  "Signal from OS",
	/* 41:MONNIK_NODE_ID_SELF          */  "Cannot send event to self",
	/* 41:MONNIK_MAX_ERROR_CODE        */  "Invalid error code"
	};

const int MonNik_nerr = sizeof(MonNik_errlist) / sizeof(char*);

char * MONNIKErrorMsg(MONNIK_STATUS error)
{

	if ((error > MONNIK_MAX_ERROR_CODE) || (error < MONNIK_SUCCESS))
		{
		return( (char*) MonNik_errlist[MONNIK_MAX_ERROR_CODE]);
		}

	return( (char*) MonNik_errlist[error]);

}





