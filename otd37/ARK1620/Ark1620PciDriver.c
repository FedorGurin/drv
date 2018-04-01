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

#include "include/Ark1620PciDriver.h"

#define VENDOR_ID_ARK1620     0x5432             /* assigned vendor ID */
#define DEVICE_ID_ARK1620     0x1620             /* Device ID     */

/* Our device name.  Make sure DEVNAME_SIZE is correct. */
LOCAL char *devNameArk1620 = "/ARK1620PCI";

/* Remember how many boards were detected in the system */
LOCAL int ark_device_count = 0;

//! задать тип РК для выходного канала 
LOCAL void SetOutRkTypes(VXB_DEVICE_ID pInst,
						 uint32_t value);/*тип команды 1-"27В", 0 - "0В"*/
//! выдать РК
LOCAL void SetOutRk(VXB_DEVICE_ID pInst,
						 uint32_t value);/*тип команды 1-"27В", 0 - "0В"*/
//! прочитать РК
LOCAL uint32_t GetInRk(VXB_DEVICE_ID pInst);
				
LOCAL void Ark1620InitDevice( VXB_DEVICE_ID pInst, UINT32 inst );
//! объявляем функции
METHOD_DECL(Ar16t16r_SetOutRkTypes);
METHOD_DECL(Ar16t16r_SetOutRk);
METHOD_DECL(Ar16t16r_GetInRk);
LOCAL struct vxbDeviceMethod Ark1620Drv_methods[] =
{
		DEVMETHOD(Ar16t16r_SetOutRkTypes, SetOutRkTypes),
	    DEVMETHOD(Ar16t16r_SetOutRk, 	  SetOutRk ),
		DEVMETHOD(Ar16t16r_GetInRk,       GetInRk ),
        DEVMETHOD_END
};

/*
 * Prototypes
 */
LOCAL void Ark1620InstInit   (VXB_DEVICE_ID pInst);
LOCAL void Ark1620InstInit2  (VXB_DEVICE_ID pInst);
LOCAL void Ark1620InstConnect(VXB_DEVICE_ID pInst);

//! функции
LOCAL struct drvBusFuncs Ark1620Funcs =
{
    Ark1620InstInit,     /* devInstanceInit */
    Ark1620InstInit2,    /* devInstanceInit2 */
    Ark1620InstConnect   /* devConnect */
};
//! идентификация устройства на шине PCI
LOCAL struct vxbPciID Ark1620PciDevIDList[] =
{
    /* { devID, vendID } */
    {   DEVICE_ID_ARK1620, VENDOR_ID_ARK1620}
};

//! структура для регистрации драйвера
LOCAL struct vxbPciRegister Ark1620DevReg =
{
{
    NULL,                       /* reserved for VxBus use */
    VXB_DEVID_DEVICE,           /* devID */
    VXB_BUSID_PCI,              /* busID = PCI */
    VXB_VER_4_0_0,              /* vxbVersion */
    "Ark1620",                 /* drvName */
    &Ark1620Funcs,             /* pDrvBusFuncs */
    &Ark1620Drv_methods[0],    /* pMethods */
    NULL,                       /* devProbe */
    NULL
},
1,
Ark1620PciDevIDList
};

//! задать тип РК для выходного канала 
LOCAL void SetOutRkTypes(VXB_DEVICE_ID pInst,				 
						 uint32_t value)/*тип команды 1-"27В", 0 - "0В"*/
{
	uint32_t baseAddr;

	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
	sysOutLong(baseAddr + REG_RK_RW,value);
	return value;		
}
//! выдать РК
LOCAL void SetOutRk(VXB_DEVICE_ID pInst,
					uint32_t value)/*тип команды 1-"27В", 0 - "0В"*/
{
	uint32_t baseAddr;

	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
	
	sysOutLong(baseAddr + REG_OUT_TYPES_RW,value);
	
	return value;	
}
//! прочитать РК
LOCAL uint32_t GetInRk(VXB_DEVICE_ID pInst)		 		    
{	
	uint32_t baseAddr;
	uint32_t value;
		
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;

	sysInLong(baseAddr + REG_RK_RW,value);
	
	return value;	
}

//! эта функция добавляется в CDF файле
void Ark1620PciRegister(void)
{
    //! признак того, что регистрацию уже выполняли
    static BOOL initCalled = FALSE;

    if (initCalled == FALSE)
    {
        vxbDevRegister((struct vxbDevRegInfo *)&Ark1620DevReg);
        initCalled = TRUE;
    }
    return;
}
LOCAL void Ark1620InstInit ( VXB_DEVICE_ID pInst )
{
    TARK1620DEVICEINFO *devInfo;
    logMsg(" Init\n ",0,0,0,0,0,0);
    if(ar_device_count >= AR16T16R_MAX_DEVICES)
    {
        return;
    }
    pInst->pDrvCtrl = (TARK1620DEVICEINFO *)hwMemAlloc(sizeof(TARK1620DEVICEINFO));
    if(pInst->pDrvCtrl == NULL)
    {
        return;
    }
    bzero((char *)pInst->pDrvCtrl, sizeof(TARK1620DEVICEINFO));
    Ark1620InitDevice( pInst, ark_device_count );

    devInfo = pInst->pDrvCtrl;

    devInfo->unit     = ark_device_count;
    devInfo->devId    = pInst;
    devInfo->instance = 0;
    ark_device_count++;

    vxbNextUnitGet (pInst);
    return;
}

LOCAL void Ark1620InstInit2( VXB_DEVICE_ID pInst )
{
    return;
}

LOCAL void Ark1620InstConnect( VXB_DEVICE_ID pInst )
{
    TARK1620CONFIG     *cfg;
    TARK1620DEVICEINFO  *devInfo = (TARK1620DEVICEINFO *)pInst->pDrvCtrl;
    //MONNIK_STATUS status;

    
    cfg  = devInfo->config;
    printf("Adapter Ark1620 = %d, BaseAddr = %x\n ",devInfo->unit,(int)cfg->pci.baseAddr,0,0,0,0,0);
    if(cfg == NULL)
    {
        return;
    }


	
//	vxbWrite32((void *)cfg->PCI.pBaseAddress,
//				(MONNIK_UINT32 *)(cfg->PCI.BaseAddress+ADRMON(RMASKINT) ),
//				(MONNIK_UINT32) 0xffffffff);


	 /* Connect Interrupt Handler */
	 //status = vxbIntConnect (pInst, 0, pciMonNikInterrupt, 0);
//	 if(status == ERROR)
//	 {
//		MONNIK_DBG(MONNIK_DBERROR,"%s: ERROR in connecting interrupt\n",0,0,0,0,0,0);
//	 }
//	 MONNIK_DBG(MONNIK_DBERROR,"%s: end() MonNikInstConnect\n",0,0,0,0,0,0);

	return;
}
LOCAL void Ark1620InitDevice( VXB_DEVICE_ID pInst, UINT32 inst )
{
	 UINT8         cInst[4];
	 UINT16        device;       /* Temp for device value                 */
	 UINT8         intNum;       /* Temp for interrupt value              */
	 UINT32        devfn;        /* Temp for devfn value                  */
	 
	 UINT8         bus;          /* Temp for interrupt value              */
	 
	 TARK1620CONFIG  *cfg = NULL;       /*   Local pointer to config structure     */
	 UINT8         devicerev;
	 UINT32        baseAddr = 0;
     UINT32    size;
     UINT32 flags;

     struct vxbPciDevice *pPciDev;

     TARK1620DEVICEINFO *pDrvCtrl = pInst->pDrvCtrl;

	    cfg = (TARK1620CONFIG *) hwMemAlloc (sizeof (TARK1620CONFIG));
	    if(cfg == NULL)
	    {
	        pDrvCtrl->config = NULL;
	        return;
	    }
	    bzero((char *)cfg, sizeof(TARK1620CONFIG));

	    cfg->size = sizeof(TARK1620CONFIG);

	    /* Get the PCI bus, function, & devfn numbers, and other easy stuff */
	    pPciDev = (struct vxbPciDevice *)pInst->pBusSpecificDevInfo;
	    cfg->pci.bus        = bus = pPciDev->pciBus;
	    cfg->pci.devfn      = devfn = pPciDev->pciDev;
	    cfg->pci.func   = pPciDev->pciFunc;
	    cfg->unit           = inst;


	    /* save the device name for this board */
	    sprintf((char *)cInst, "%d", inst);
	    //strcat((char *)cfg->device,(const char *) devnameMonNik);
	    strcat((char *)cfg->device,(const char *) "_");
	    strcat((char *)cfg->device,(const char *) cInst);


	    /* Get the device ID*/
	    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_DEVICE_ID, 2, device,&flags );
	    cfg->pci.type = device;

	    /* Get the device revision */
	    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_REVISION, 1, devicerev, &flags);

	    
	    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_BASE_ADDRESS_2, 4, baseAddr, &flags);
	    cfg->pci.baseAddr = baseAddr;
	    
	    pDrvCtrl->config = (TARK1620CONFIG *) cfg;

	    return;   
}








