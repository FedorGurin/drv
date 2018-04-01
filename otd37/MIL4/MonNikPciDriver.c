/* gefRfm2gPciDrv.c - Driver source code for GE Fanuc PCI RFM2g device */

/*
===============================================================================
                            COPYRIGHT NOTICE

    Copyright (C) 2009 GE Fanuc Embedded Systems, Inc.
    International Copyright Secured.  All Rights Reserved.

-------------------------------------------------------------------------------

modification history
--------------------
$Workfile: MonNikPciDrv.c $
$Revision: $
$Modtime: $
*/

/*
DESCRIPTION


INCLUDE FILES:
*/

/* Include header files */
#include <vxWorks.h>
#include <intLib.h>
#include <stdio.h>
#include <string.h>
#include <logLib.h>
#include <vxBusLib.h>
#include <lstLib.h>
#include <sysLib.h>
#include <vmLib.h>
#include <vxAtomicLib.h>

#include <hwif/util/hwMemLib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPciLib.h>
#include <hwif/vxbus/hwConf.h>
#include <hwif/util/vxbParamSys.h>
#include <../src/hwif/h/vxbus/vxbAccess.h>
#include <../src/hwif/h/vxbus/vxbArchAccess.h>

/* VxWorks provided PCI header files */
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciHeaderDefs.h"
#include "drv/pci/pciIntLib.h"

#include "include/MonnikPciDriver.h"


/****************************************DEBUG Stuff************************************************/

/* debug */

#define MONNIK_DEBUG_ENABLE
#ifdef MONNIK_DEBUG_ENABLE
#ifndef MONNIK_DBG
#   define MONNIK_DBG(level,fmt,a,b,c,d,e,f) \
				   if ( MONNIK_DEBUG_STATUS & level ) \
					   logMsg(fmt,a,b,c,d,e,f)
#endif /* GEF_RFM2G_DBG */
#else
#   define MONNIK_DBG(level,fmt,a,b,c,d,e,f)
#endif

/* DEBUG Flag information */

static MONNIK_UINT32 MONNIK_DEBUG_STATUS = 1;

/****************************************DEBUG Stuff END*********************************************/
/* Macros required for sysMmuMapAdd on PPC BSPs */
#ifndef VM_STATE_MASK_FOR_ALL
#define VM_STATE_MASK_FOR_ALL	VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY
#endif

#ifndef VM_STATE_FOR_PCI
#define VM_STATE_FOR_PCI     VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
#endif

/* Device IDs of supported PCI, PMC, and CPCI Reflective Memory boards       */
#define VENDOR_ID_MONNIK     0x1172             /*VEN_1172&DEV_0006    assigned vendor ID */
#define DEVICE_ID_MONNIK     0x0006             /*VEN_1172&DEV_0006   Device ID     */

/* Sliding Memory Windows */
/*
 * The first board revision that has support for
 * Sliding Memory Windows in the firmware
 */


#define MONNIK_PRODUCT_STRING     "SFT-MONNIK-VXW"
#define MONNIK_PRODUCT_VERSION    "R01.00"

#define GEF_SWAP16(A) ((((UINT16)(A) & 0xff00) >> 8) | \
					   (((UINT16)(A) & 0x00ff) << 8))
#define GEF_SWAP32(A) ((((UINT32)(A) & 0xff000000) >> 24) | \
					   (((UINT32)(A) & 0x00ff0000) >> 8)  | \
					   (((UINT32)(A) & 0x0000ff00) << 8)  | \
					   (((UINT32)(A) & 0x000000ff) << 24))
#define GEF_SWAP64(x) (((UINT64)(GEF_SWAP32((UINT32)((x << 32) >> 32))) << 32) |  \
					   (UINT32)GEF_SWAP32(((UINT32)(x >> 32))))


/*************************GLOBALS************************/


/* Our device name.  Make sure DEVNAME_SIZE is correct. */
LOCAL char *devnameMonNik = "/MONNIKPCI";

/* Remember how many boards were detected in the system */
LOCAL MONNIK_INT32 MonNik_device_count = 0;

/*
 * Driver method
 */

LOCAL STATUS pciMonNikDevice (VXB_DEVICE_ID pInst, MONNIK_UINT32 *monnikType);

LOCAL STATUS pciMonNikOpen (VXB_DEVICE_ID pInst);
LOCAL STATUS pciMonNikClose (VXB_DEVICE_ID pInst);
LOCAL STATUS pciMonNikGetConfig (VXB_DEVICE_ID pInst,
								MONNIKCONFIG *pCfg);

LOCAL STATUS pciMonNikMapUserMemory (VXB_DEVICE_ID pInst,
								  volatile void **UserMemoryPtr,
								  MONNIK_UINT64 Offset,
								  MONNIK_UINT32 Pages);

LOCAL STATUS pciMonNikUnMapUserMemory (VXB_DEVICE_ID pInst,
									volatile void **UserMemoryPtr,
									MONNIK_UINT32 Pages);



LOCAL STATUS pciMonNikSize (VXB_DEVICE_ID pInst,
					MONNIK_UINT32 *SizePtr);

LOCAL STATUS pciMonNikDeviceName (VXB_DEVICE_ID pInst,
								 MONNIK_UINT8* name);


LOCAL STATUS pciMonNikDriverVersion (VXB_DEVICE_ID pInst,
									MONNIK_UINT8* version);

LOCAL STATUS pciMonNikPeek32 (VXB_DEVICE_ID pInst,
							 MONNIK_UINT32    offset,
							 MONNIK_UINT32 *  pData);
LOCAL STATUS pciMonNikPoke32 (VXB_DEVICE_ID pInst,
							 MONNIK_UINT32    offset,
							 MONNIK_UINT32    data);
LOCAL STATUS pciMonNikGetDebugFlags (VXB_DEVICE_ID pInst,
									MONNIK_UINT32 *Flags);
LOCAL STATUS pciMonNikSetDebugFlags (VXB_DEVICE_ID pInst,
									MONNIK_UINT32 Flags);


LOCAL STATUS pciMonNikMapDeviceRegistr(VXB_DEVICE_ID pInst,  unsigned int    *DeviceBaseRegistr);

LOCAL STATUS pciMonNikSetAdrInterrupFuction(VXB_DEVICE_ID pInst,   unsigned int  AdrFunctin );



/*******************************************VxBus Model Stuff******************************************/
METHOD_DECL(ikiranMonNikDevice);
METHOD_DECL(ikiranMonNikOpen);
METHOD_DECL(ikiranMonNikClose);
METHOD_DECL(ikiranMonNikGetConfig);
METHOD_DECL(ikiranMonNikMapUserMemory);
METHOD_DECL(ikiranMonNikUnMapUserMemory);
METHOD_DECL(ikiranMonNikSize);
METHOD_DECL(ikiranMonNikDeviceName);
METHOD_DECL(ikiranMonNikDriverVersion);
METHOD_DECL(ikiranMonNikPeek32);
METHOD_DECL(ikiranMonNikPoke32);
METHOD_DECL(ikiranMonNikGetDebugFlags);
METHOD_DECL(ikiranMonNikSetDebugFlags);
METHOD_DECL(ikiranMonNikMapDeviceRegistr);
METHOD_DECL(ikiranMonNikSetAdrInterrupFuction);

LOCAL struct vxbDeviceMethod ikiranMonNikDrv_methods[] =
{
	DEVMETHOD(ikiranMonNikDevice, pciMonNikDevice),
	DEVMETHOD(ikiranMonNikOpen, pciMonNikOpen),
	DEVMETHOD(ikiranMonNikClose, pciMonNikClose),
	DEVMETHOD(ikiranMonNikGetConfig, pciMonNikGetConfig),
	DEVMETHOD(ikiranMonNikMapUserMemory, pciMonNikMapUserMemory),
	DEVMETHOD(ikiranMonNikUnMapUserMemory, pciMonNikUnMapUserMemory),
	DEVMETHOD(ikiranMonNikSize, pciMonNikSize),
	DEVMETHOD(ikiranMonNikDeviceName, pciMonNikDeviceName),
	DEVMETHOD(ikiranMonNikDriverVersion, pciMonNikDriverVersion),
	DEVMETHOD(ikiranMonNikPeek32, pciMonNikPeek32),
	DEVMETHOD(ikiranMonNikPoke32, pciMonNikPoke32),
	DEVMETHOD(ikiranMonNikGetDebugFlags, pciMonNikGetDebugFlags),
	DEVMETHOD(ikiranMonNikSetDebugFlags, pciMonNikSetDebugFlags),

	DEVMETHOD(ikiranMonNikMapDeviceRegistr, pciMonNikMapDeviceRegistr),

	DEVMETHOD(ikiranMonNikSetAdrInterrupFuction,pciMonNikSetAdrInterrupFuction),

	{0,NULL}
};

/*
 * Prototypes
 */
extern STATUS sysMmuMapAdd(void*, UINT, UINT, UINT);

LOCAL void MonNikInstInit(VXB_DEVICE_ID pInst);
LOCAL void MonNikInstInit2(VXB_DEVICE_ID pInst);
LOCAL void MonNikInstConnect(VXB_DEVICE_ID pInst);

static void pciMonNikInterrupt (	VXB_DEVICE_ID pInst );
LOCAL void MonNikInitDevice( VXB_DEVICE_ID pInst, MONNIK_UINT32 inst );


LOCAL struct drvBusFuncs MonNikFuncs =
{
	MonNikInstInit, 				/* devInstanceInit */
	MonNikInstInit2, 				/* devInstanceInit2 */
	MonNikInstConnect 				/* devConnect */
};
LOCAL struct vxbPciID MonNikPciDevIDList[] =
{
	/* { devID, vendID } */
	{	DEVICE_ID_MONNIK, VENDOR_ID_MONNIK},
};

LOCAL struct vxbPciRegister MonNikDevRegistration =
{
	{
		NULL,					/* reserved for VxBus use */
		VXB_DEVID_BUSCTRL,		/* devID */
		VXB_BUSID_PCI,			/* busID = PCI */
#ifdef	VXB_LEGACY_ACCESS
		VXBUS_VERSION_3, 		/* vxbVersion */
#else
		VXB_VER_4_0_0,   		/* vxbVersion */
#endif
		"NikMonitor",				/* drvName */
		&MonNikFuncs, 		/* pDrvBusFuncs */
		&ikiranMonNikDrv_methods[0],/* pMethods */
		NULL, 					/* devProbe */
		NULL
	},
	1,
	MonNikPciDevIDList
};

#ifdef	VXB_LEGACY_ACCESS
	#define PCI_BUS_CFG_READ(xxxInst, xxxReg, xxxSize, xxxBuffer, xxxFlags) \
		(xxxInst->pAccess->busCfgRead(xxxInst, xxxReg, xxxSize, (char*)&xxxBuffer, xxxFlags))

	#define PCI_BUS_CFG_WRITE(xxxInst, xxxReg, xxxSize, xxxBuffer, xxxFlags) \
		(xxxInst->pAccess->busCfgWrite(xxxInst, xxxReg, xxxSize, (char*)&xxxBuffer, xxxFlags))

#else
	#define PCI_BUS_CFG_READ(xxxInst, xxxReg, xxxSize, xxxBuffer, xxxFlags) \
		VXB_PCI_BUS_CFG_READ(xxxInst, xxxReg, xxxSize, xxxBuffer)

	#define PCI_BUS_CFG_WRITE(xxxInst, xxxReg, xxxSize, xxxBuffer, xxxFlags) \
		VXB_PCI_BUS_CFG_WRITE(xxxInst, xxxReg, xxxSize, xxxBuffer)
#endif

/******************************************VxBus Model END****************************************************/

/*
 * Get the size of BAR
 */
__inline__ LOCAL STATUS pciMonNikBarSizeGet(VXB_DEVICE_ID pInst, UINT8 bar, UINT32* size)
{
			UINT32 regVal = 0;
			UINT32 regValTemp = 0xFFFFFFFF;
			STATUS status = ERROR;
			/*MONNIK_UINT32 flags; */

			status = PCI_BUS_CFG_READ (pInst, PCI_CFG_BASE_ADDRESS_0 + (4 * bar),
												 4, regVal, &flags);
			if(status == ERROR)
						return ERROR;
			status = PCI_BUS_CFG_WRITE(pInst, PCI_CFG_BASE_ADDRESS_0 + (4 * bar),
												 4, regValTemp,&flags);
			if(status == ERROR)
						return ERROR;
			status = PCI_BUS_CFG_READ (pInst, PCI_CFG_BASE_ADDRESS_0 + (4 * bar),
												 4, regValTemp,&flags);
			if(status == ERROR)
						return ERROR;
			status = PCI_BUS_CFG_WRITE(pInst, PCI_CFG_BASE_ADDRESS_0 + (4 * bar),
												 4, regVal,&flags);
			if(status == ERROR)
						return ERROR;

			if ((regVal & PCI_BAR_SPACE_MASK) == PCI_BAR_SPACE_IO)
						regValTemp &= PCI_IOBASE_MASK;
			else
						regValTemp &= PCI_MEMBASE_MASK;
			/* reuse the regVal variable */
			regVal = 0;

			if(regValTemp != 0)
			{
				 while((regValTemp & 0x01) == 0)
				 {
					   regVal++;
					   regValTemp >>= 1;
				 }
			}
			*size = 0x01 << regVal;
			return status;

}

__inline__ LOCAL MONNIK_STATUS pciMonNikValidateDevId(MONNIKDEVICEINFO * pDrvCtrl)
{
	if(pDrvCtrl == NULL)
	{
		return MONNIK_DRIVER_ERROR;
	}
	if(pDrvCtrl->Config == NULL)
	{
		return MONNIK_DRIVER_ERROR;
	}
	return MONNIK_SUCCESS;
}

void MonNikPciRegister(void)
{
	static BOOL MonNikInitCalled = FALSE;

	MONNIK_DBG(MONNIK_DBERROR,"%s: start MonNikPciRegister\n",0,0,0,0,0,0);
	   logMsg("MonNikPciRegister :  registered begin\n",0,0,0,0,0,0);

	if (MonNikInitCalled)
	{
		MONNIK_DBG(MONNIK_DBINIT,"MonNikPciRegister : Already registered\n",0,0,0,0,0,0);
	}
	else
	{
		MONNIK_DBG(MONNIK_DBINIT,"MonNikPciRegister : Already registered!\n",0,0,0,0,0,0);
		vxbDevRegister((struct vxbDevRegInfo *)&MonNikDevRegistration);
		MonNikInitCalled = TRUE;
	}
	MONNIK_DBG(MONNIK_DBINIT,"MonNikPciRegister :  registered passed\n",0,0,0,0,0,0);

	return;
}

LOCAL void MonNikInstInit ( VXB_DEVICE_ID pInst )
{
	MONNIKDEVICEINFO *devInfo;

	MONNIK_DBG(MONNIK_DBERROR,"%s: start MonNikInstInit\n",0,0,0,0,0,0);
	if(MonNik_device_count >= MONNIK_MAX_DEVICES)
	{
		return;
	}

	pInst->pDrvCtrl = (MONNIKDEVICEINFO *)hwMemAlloc(sizeof(MONNIKDEVICEINFO));
	if(pInst->pDrvCtrl == NULL)
	{
		return;
	}
	bzero((char *)pInst->pDrvCtrl, sizeof(MONNIKDEVICEINFO));

	MonNikInitDevice( pInst, MonNik_device_count );

	devInfo = pInst->pDrvCtrl;
	if(devInfo->Config == NULL)
	{
		return;
	}
	devInfo->unit     = MonNik_device_count;
	devInfo->devId    = pInst;
	devInfo->Instance = 0;

	MonNik_device_count++;

	vxbNextUnitGet (pInst);

	return;
}

LOCAL void MonNikInstInit2( VXB_DEVICE_ID pInst )
{
	MONNIK_DBG(MONNIK_DBERROR,"MonNikInstInit2 :  begin\n",0,0,0,0,0,0);
	return;
}

LOCAL void MonNikInstConnect( VXB_DEVICE_ID pInst )
{
	/*char *me = "MonNikInstConnect"; */
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"%s: start MonNikInstConnect\n",0,0,0,0,0,0);
	cfg  = devInfo->Config;
	if(cfg == NULL)
	{
		return;
	}

	printf("BaseAdr=%x \n AdR=%X " ,(int)cfg->PCI.BaseAddress,(MONNIK_UINT32)cfg->PCI.BaseAddress+ADRMON(RMASKINT));
	/* disable all interrupts */

	MONNIK_DBG(MONNIK_DBERROR,"%s: Just Test in Debugging message\n",0,0,0,0,0,0);
	
	vxbWrite32((void *)cfg->PCI.pBaseAddress,
				(MONNIK_UINT32 *)(cfg->PCI.BaseAddress+ADRMON(RMASKINT) ),
				(MONNIK_UINT32) 0xffffffff);


	 /* Connect Interrupt Handler */
	 //status = vxbIntConnect (pInst, 0, pciMonNikInterrupt, 0);
//	 if(status == ERROR)
//	 {
//		MONNIK_DBG(MONNIK_DBERROR,"%s: ERROR in connecting interrupt\n",0,0,0,0,0,0);
//	 }
	 MONNIK_DBG(MONNIK_DBERROR,"%s: end() MonNikInstConnect\n",0,0,0,0,0,0);

	return;
}
LOCAL void MonNikInitDevice( VXB_DEVICE_ID pInst, MONNIK_UINT32 inst )
{
	
	MONNIK_UINT8         cInst[4];
	MONNIK_UINT16        device;       /* Temp for device value                 */
	MONNIK_UINT8         intNum;       /* Temp for interrupt value              */
	MONNIK_UINT32        devfn;        /* Temp for devfn value                  */
	/*	unsigned long       IntFlags = 0;  Used to save status of interrupts     */
	MONNIK_UINT8         bus;          /* Temp for interrupt value              */
	 /*MONNIK_UINT8         boardId;      Temp for RFM board ID                 */
	MONNIKCONFIGVXWORKS  *cfg;       /*   Local pointer to config structure     */
	MONNIK_UINT8         devicerev;
	MONNIK_ADDR          BaseAddr = 0;
	void *          MappedAddr = 0;
	/*MONNIK_UINT32        flags; */
	MONNIK_UINT32 	    size;
	struct vxbPciDevice *pPciDev;

	static char *me = "MonNikInitDevice()";
	
	MONNIKDEVICEINFO *pDrvCtrl = pInst->pDrvCtrl;
	//MONNIK_DBG(MONNIK_DBINIT,"MonNikMonNikInitDevice :  begin\n",0,0,0,0,0,0);
	MONNIK_DBG(MONNIK_DBERROR,"MonNikMonNikInitDevice :  begin\n",0,0,0,0,0,0);

	

	// = "MonNikInitDevice()";
	
	cfg = (MONNIKCONFIGVXWORKS *) hwMemAlloc (sizeof (MONNIKCONFIGVXWORKS));
	if(cfg == NULL)
	{
		pDrvCtrl->Config = NULL;
		return;
	}
	bzero((char *)cfg, sizeof(MONNIKCONFIGVXWORKS));

	cfg->Size = sizeof(MONNIKCONFIGVXWORKS);

	 /* Get the PCI bus, function, & devfn numbers, and other easy stuff */
	  pPciDev = (struct vxbPciDevice *)pInst->pBusSpecificDevInfo;
	  cfg->PCI.bus = bus = pPciDev->pciBus;
	  cfg->PCI.devfn = devfn = pPciDev->pciDev;
	  cfg->PCI.function = pPciDev->pciFunc;
	  cfg->Unit = inst;


	  /* save the device name for this board */
	  sprintf((char *)cInst, "%d", inst);
	  strcat((char *)cfg->Device,(const char *) devnameMonNik);
	  strcat((char *)cfg->Device,(const char *) "_");
	  strcat((char *)cfg->Device,(const char *) cInst);


	  /* Get the device ID, e.g. 0x5565 */
	  PCI_BUS_CFG_READ (pInst, PCI_CFG_DEVICE_ID, 2, device,&flags );
	  cfg->PCI.type = device;

	  /* Get the device revision */
	  PCI_BUS_CFG_READ (pInst, PCI_CFG_REVISION, 1, devicerev, &flags);

	  /* Get the interrupt number */
	  PCI_BUS_CFG_READ (pInst, PCI_CFG_DEV_INT_LINE, 1, intNum, &flags);

	  cfg->PCI.interruptNumber = intNum;

	  MONNIK_DBG(MONNIK_DBINIT,"%s: MonNik device:0x%x\nRfm2g revision:0x%x\n MonNik interrupt:0x%x\n",
				  (int)me, cfg->PCI.type, 1, cfg->PCI.interruptNumber, 0, 0);
/*
	   Get the PCI physical address for the local, runtime and DMA Registers
		on the RFM board (rfmor) */
	  /* Get the size of the RFM Memory area */
	 cfg->PCI.BaseAddress = (MONNIK_UINT32)((MONNIK_UINT8 *)pInst->pRegBase[0]);
	  /* Get the size of the RFM Memory area */
	  pciMonNikBarSizeGet(pInst, 0, &size);
	  cfg->PCI.MonNikWindowSize = size;
	  printf("!!!!!size=%d\n",size);

	  /*BaseAddr =(MONNIK_ADDR) cfg->PCI.pBaseAddress;*/
	  
	 vxbRegMap (pInst, 0, (void **)&MappedAddr);
	
	 /* MappedAddr=(void *)BaseAddr; */
	 cfg->PCI.pBaseAddress = (MONNIK_UINT8 *) MappedAddr;
	 cfg->PCI.MonNikWindowSize=size;

/*	 MONNIK_DBG(MONNIK_DBINIT,"%s: MonNikBase:0x%x Size=x%X \n",(int)me,(int) cfg->PCI.pBaseAddress,(int) cfg->PCI.MonNikWindowSize,0,0,0);

	 Set MMU Attributes for the device to be accessed from RTP 
 
	sysMmuMapAdd((void *)cfg->PCI.rfm2gOrBase,
				cfg->PCI.rfm2gOrWindowSize,
				(VM_STATE_MASK_FOR_ALL),
				(VM_STATE_FOR_PCI |
				MMU_ATTR_USR_RWX));

	sysMmuMapAdd((void *)cfg->PCI.rfm2gCsBase,
				cfg->PCI.rfm2gCsWindowSize,
				(VM_STATE_MASK_FOR_ALL),
				(VM_STATE_FOR_PCI |
				MMU_ATTR_USR_RWX));
*/
	sysMmuMapAdd((void *)cfg->PCI.BaseAddress,
				cfg->PCI.MonNikWindowSize,
				(VM_STATE_MASK_FOR_ALL),
				(VM_STATE_FOR_PCI |
				MMU_ATTR_USR_RWX));


	pDrvCtrl->Config = (MONNIKCONFIGVXWORKS *) cfg;

	MONNIK_DBG(MONNIK_DBINIT,"MonNikMonNikInitDevice :  last\n",0,0,0,0,0,0);
	
	 return;
}

/**************************************************************************
*
* pciRfm2gInterrupt - Handles the interrupts of ther RFM2g Card
*
* Handles the interrupts of ther RFM2g Card
*
* RETURNS: RFM2G_STATUS
*/

static void pciMonNikInterrupt
	(
			VXB_DEVICE_ID pInst
	)
	{
	char *me = "pciMonNikInterrupt";
	/*MONNIK_UINT32  dINTCSR;*/
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikInterrupt :  begin\n",0,0,0,0,0,0);
	cfg  = devInfo->Config;
   ((VOIDFUNCPTR )(cfg->PCI.InterruptAdr))();

	/* Read Interrupt Status register
	dINTCSR = vxbRead32((void *)cfg->pOrRegisters,
			  (RFM2G_UINT32 *)(cfg->PCI.rfm2gOrBase + rfmor_intcsr));


		MONNIK_DBG(MONNIK_DBINTR,"%s: Tier 2 interrupt detected\n",(int)me, 0,0,0,0,0);

	 Flush out writes to clear interrupts by reading
	dINTCSR = vxbRead32((void *)cfg->pOrRegisters,
			  (MONNIK_UINT32 *)(cfg->PCI.rfm2gOrBase + rfmor_intcsr));

	Unlock register accesses.

  */

}

LOCAL STATUS
pciMonNikDevice (VXB_DEVICE_ID pInst, MONNIK_UINT32 *rfmType)
{
	*rfmType = 0;
	return OK; /* RFM2G_BUS_PCI */
}

/*************************************Local Helper Functions**************************************/



/**************************************************************************
*
* pciMonNikOpen - Open and init the RFM2g device
*
* Open and init the RFM2g device.
*
* RETURNS: RFM2G_STATUS
*/
LOCAL STATUS
pciMonNikOpen( VXB_DEVICE_ID pInst )
{
	char *me = "pciMonNikOpen()";
	MONNIK_ADDR BaseAddr = 0;
	MONNIK_ADDR MappedAddr = 0;
	atomicVal_t value;
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikOpen :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;

	/* Count how many times we've been opened */
	value = vxAtomicInc(&devInfo->Instance);
	if(1 == vxAtomicGet(&devInfo->Instance))
	{
		MONNIK_DBG(MONNIK_DBOPEN,"%s: Opened device for first time...Initializing Interrupts\n",
						(int)me, 0,0,0,0,0);
		/* Enable the PCI interrupt level */
		vxbIntEnable(pInst, 0, pciMonNikInterrupt, pInst);


	}
	MONNIK_DBG(MONNIK_DBOPEN,"%s: Total open instances: %d\n",
					(int)me, ++value,0,0,0,0);

	return MONNIK_SUCCESS;
}   /* End of pciMonNikOpen() */

/**************************************************************************
*
* pciMonNikClose - Close down the handle created with pciMonNikOpen
*
* Close down the handle created with pciMonNikOpen
*
* RETURNS: RFM2G_STATUS
*/
LOCAL STATUS
pciMonNikClose( VXB_DEVICE_ID pInst)
{
	char *me = "pciMonNikClose()";
	MONNIK_ADDR BaseAddr = 0;
	MONNIK_ADDR MappedAddr = 0;
	atomicVal_t value;
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikClose :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;

	 /* Count how many times we've been closed */
	value = vxAtomicDec(&devInfo->Instance);
	if(0 == vxAtomicGet(&devInfo->Instance))
	{
		MONNIK_DBG(MONNIK_DBCLOSE,"%s: Closing the last device instance...Resetting Interrupts\n",
						(int)me, 0,0,0,0,0);

	 }
	return(MONNIK_SUCCESS);

}

LOCAL STATUS pciMonNikGetConfig (VXB_DEVICE_ID pInst, MONNIKCONFIG *pCfg)
{
	char *me = "pciMonNikGetConfig()";
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikGetConfig :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;


	/* Copy the common stuff over */
	pCfg->Unit                = cfg->Unit;

	strcpy((char *)pCfg->Device, (const char *)cfg->Device);
	strcpy(pCfg->Name, MONNIK_PRODUCT_STRING);
	strcpy(pCfg->DriverVersion, MONNIK_PRODUCT_VERSION);
	memcpy(&pCfg->PciConfig, &cfg->PCI, sizeof(MONNIKPCICONFIG));


	return OK;
}


LOCAL STATUS pciMonNikSize (VXB_DEVICE_ID pInst, MONNIK_UINT32 *SizePtr)
{
	char *me = "pciMonNikSize()";
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikSize :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;


	*SizePtr = cfg->PCI.MonNikWindowSize;
	return OK;
}

LOCAL STATUS pciMonNikDeviceName (VXB_DEVICE_ID pInst, MONNIK_UINT8* name)
{
	char *me = "pciMonNikDeviceName()";
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikDeviceName :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;


	strcpy((char *)name, (const char *)cfg->Device);
	return OK;
}

LOCAL STATUS pciMonNikDriverVersion (VXB_DEVICE_ID id, MONNIK_UINT8* version)
{
	strcpy((char *)version, MONNIK_PRODUCT_VERSION);
	return OK;
}


LOCAL STATUS pciMonNikPeek32 (VXB_DEVICE_ID pInst,

							 MONNIK_UINT32    offset,
							 MONNIK_UINT32 *  pData)
{
	char *me = "pciMonNikPeek32()";

	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_UINT32 rfmSize  = 0;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikPeek32 :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;

	rfmSize = cfg->PCI.MonNikWindowSize;

	if ((offset + sizeof(MONNIK_UINT32)  > rfmSize))
	{
		return MONNIK_OUT_OF_RANGE;
	}
	if ((offset % sizeof(MONNIK_UINT32)) != 0)
	{
		return (MONNIK_UNALIGNED_OFFSET);
	}

	*pData = vxbRead32((void *)cfg->PCI.pBaseAddress,
					  (MONNIK_UINT32 *)(cfg->PCI.BaseAddress + offset));


	return MONNIK_SUCCESS;
}
LOCAL STATUS pciMonNikPoke32 (VXB_DEVICE_ID pInst,
							 MONNIK_UINT32    offset,
							 MONNIK_UINT32    data)
{
	char *me = "pciMonNikPoke32()";
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_UINT32 rfmSize  = 0;
	MONNIK_UINT32 pData = data;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikPoke32 :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;

	rfmSize = cfg->PCI.MonNikWindowSize;

	if ((offset + sizeof(MONNIK_UINT32)  > rfmSize))
	{
		return MONNIK_OUT_OF_RANGE;
	}
	if ((offset % sizeof(MONNIK_UINT32)) != 0)
	{
		return (MONNIK_UNALIGNED_OFFSET);
	}

	vxbWrite32((void *)cfg->PCI.pBaseAddress,
			  (MONNIK_UINT32 *)(cfg->PCI.BaseAddress + offset),
			  pData);

	return MONNIK_SUCCESS;
}


LOCAL STATUS pciMonNikMapUserMemory (VXB_DEVICE_ID pInst,
								  volatile void **UserMemoryPtr,
								  MONNIK_UINT64 Offset,
								  MONNIK_UINT32 Pages)
{
	char *me = "pciMonNikMapUserMemory()";
	MONNIKCONFIGVXWORKS *cfg;
	MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
	MONNIK_UINT32 rfmSize, pageSize;
	MONNIK_STATUS status;

	MONNIK_DBG(MONNIK_DBERROR,"pciMonNikMapUserMemory :  begin\n",0,0,0,0,0,0);
	if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
	{
		return status;
	}
	cfg  = devInfo->Config;

	rfmSize = cfg->PCI.MonNikWindowSize;

	pageSize = vmPageSizeGet();

	if((Offset > rfmSize) || (Offset + (Pages * pageSize) > rfmSize))
	{
		return MONNIK_OUT_OF_RANGE;
	}
	else
	{
		*UserMemoryPtr = (void*) (cfg->PCI.BaseAddress + (MONNIK_UINT32)Offset);
	}
	return MONNIK_SUCCESS;
}

LOCAL STATUS pciMonNikUnMapUserMemory (VXB_DEVICE_ID pInst,
									volatile void **UserMemoryPtr,
									MONNIK_UINT32 Pages)
{
	*UserMemoryPtr = NULL;
	return OK;
}




LOCAL STATUS pciMonNikGetDebugFlags (VXB_DEVICE_ID pInst,  MONNIK_UINT32 *Flags)
{

	*Flags = MONNIK_DEBUG_STATUS;
	return MONNIK_SUCCESS;
}

LOCAL STATUS pciMonNikSetDebugFlags (VXB_DEVICE_ID pInst,  MONNIK_UINT32 Flags)
{
	MONNIK_DEBUG_STATUS = Flags;
	return MONNIK_SUCCESS;

}



LOCAL STATUS pciMonNikMapDeviceRegistr(VXB_DEVICE_ID pInst,

									  unsigned int    *DeviceBaseRegistr)
{
/*         I/0 */

		MONNIKCONFIGVXWORKS *cfg;
		MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
		MONNIK_STATUS status;
		MONNIK_DBG(MONNIK_DBERROR,"pciMonNikMapDeviceRegistr :  begin\n",0,0,0,0,0,0);

		if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
		{
			return status;
		}
		cfg  = devInfo->Config;
	*DeviceBaseRegistr = ( (MONNIK_UINT32) cfg->PCI.MonNikBaseReg);
		return(MONNIK_SUCCESS);
}



LOCAL STATUS pciMonNikSetAdrInterrupFuction(VXB_DEVICE_ID pInst,

									   unsigned int  AdrFunctin )
{
/*         I/0 */

		MONNIKCONFIGVXWORKS *cfg;
		MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
		MONNIK_STATUS status;

		MONNIK_DBG(MONNIK_DBERROR,"pciMonNikSetAdrInterrupFuction :  begin\n",0,0,0,0,0,0);
		if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
		{
			return status;
		}
		cfg  = devInfo->Config;
		cfg->PCI.InterruptAdr=AdrFunctin;
		return(MONNIK_SUCCESS);
}


/*
LOCAL STATUS pciMonNikMapDeviceMemory(VXB_DEVICE_ID pInst,
									 MONNIKREGSETTYPE 	 regset,
									 volatile void   **DeviceMemoryPtr)
{
		char *me = "pciMonNikMapDeviceMemory()";
		MONNIKCONFIGVXWORKS *cfg;
		MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
		MONNIK_STATUS status;

		if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
		{
			return status;
		}
		cfg  = devInfo->Config;
	*DeviceMemoryPtr = (void*) ( (MONNIK_UINT32) cfg->PCI.pBaseAddress);
		return(MONNIK_SUCCESS);
}

LOCAL STATUS pciMonNikUnmapDeviceMemory(VXB_DEVICE_ID pInst,
									   MONNIKREGSETTYPE 	 regset,
									   volatile void   **DeviceMemoryPtr)
{
		char *me = "pciMonNikUnmapDeviceMemory()";
		MONNIKCONFIGVXWORKS *cfg;
		MONNIKDEVICEINFO *devInfo = (MONNIKDEVICEINFO *)pInst->pDrvCtrl;
		MONNIK_STATUS status;

		if ((status = pciMonNikValidateDevId(devInfo)) != MONNIK_SUCCESS)
		{
			return status;
		}
		cfg  = devInfo->Config;

		if(*DeviceMemoryPtr == NULL)
		{
			return(MONNIK_BAD_PARAMETER_2);
		}

		*DeviceMemoryPtr = NULL;

		return(MONNIK_SUCCESS);
}

*/
