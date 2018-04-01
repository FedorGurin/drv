/* Include header files */
#include <vxWorks.h>
#include <intLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logLib.h>
#include <vxBusLib.h>
#include <lstLib.h>
#include <sysLib.h>
#include <vmLib.h>
#include <fcntl.h>
#include <unistd.h>


//#include <vxbDelayLib>
#include <hwif/util/hwMemLib.h>
#include <vxbTimerLib.h>
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

#include "include/Ar16t16rPciDriver.h"
#include "include/Ar16t16r_regs.h"

#define VENDOR_ID_AR16T16R     0x4321             /* assigned vendor ID */
#define DEVICE_ID_AR16T16R     0xF771             /* Device ID     */
#define MAX_FILESIZE_BYTE  	   200000
#define FIRMWARE_CFG_FILE_PATH  "/romfs/artmr16.bin"
//#define USE_HEX_FILE  0
//!     
const  uint16_t    ARBUF_REC[16] =
{0x8000,0x8100,0x8200,0x8300,
 0x8400,0x8500,0x8600,0x8700,
 0x8800,0x8900,0x8a00,0x8b00,
 0x8c00,0x8d00,0x8e00,0x8f00};
//!     
const  uint16_t    ARBUF_TRANS[16]=
{0x1000,0x1100,0x1200,0x1300,
 0x1400,0x1500,0x1600,0x1700,
 0x1800,0x1900,0x1a00,0x1b00,
 0x1c00,0x1d00,0x1e00,0x1f00};

/* Our device name.  Make sure DEVNAME_SIZE is correct. */
LOCAL char *devNameAr16t16r = "/AR16T16RPCI";

/* Remember how many boards were detected in the system */
LOCAL int ar_device_count = 0;

//!  
LOCAL void loadFirmware(VXB_DEVICE_ID pInst);

LOCAL void flashFirmwareAr16t16r(VXB_DEVICE_ID pInst,
								 uint8_t* binData,		/*coverted data from HEX to BIN*/	
								 uint32_t sizeBytes );	/*size of BIN data*/
LOCAL uint32_t readLongWordAr16t16r(VXB_DEVICE_ID pInst,
									uint8_t io, /*  ((0)/(1)*/
									uint8_t nk, /*  0..15 ( 1*/
									uint8_t indexAddr);/*  0..*/
LOCAL void writeLongWordAr16t16r(VXB_DEVICE_ID pInst,
						 uint8_t io, /*  ((0)/(1)*/
						 uint8_t nk, /*  0..15 ( 1*/
						 uint8_t indexAddr,/*  0..*/
						 uint32_t data);/* 32 */
LOCAL void setStartAr16t16r(VXB_DEVICE_ID pInst,
							uint8_t io,		/*  ((0)/(1)*/
							uint8_t nk, 	/* */
							uint8_t value);
LOCAL void setModeAr16t16r(VXB_DEVICE_ID pInst,
						   uint8_t io,		/*  ((0)/(1)*/
						   uint8_t nk, 		/* */
						   uint32_t regime, /* */
						   uint8_t numAddr,	 /*- */
						   uint8_t *listAddr);/* */						   

LOCAL void Ar16t16rInitDevice( VXB_DEVICE_ID pInst, UINT32 inst );

//!  
METHOD_DECL(Ar16t16r_FlashFirmware);
METHOD_DECL(Ar16t16r_ReadLongWord);
METHOD_DECL(Ar16t16r_WriteLongWord);
METHOD_DECL(Ar16t16r_SetMode);
METHOD_DECL(Ar16t16r_SetStart);

//!  
LOCAL struct vxbDeviceMethod Ar16t16rDrv_methods[] =
{	
	    DEVMETHOD(Ar16t16r_FlashFirmware, loadFirmware),
	    DEVMETHOD(Ar16t16r_ReadLongWord, readLongWordAr16t16r ),
	    DEVMETHOD(Ar16t16r_WriteLongWord,writeLongWordAr16t16r ),
	    DEVMETHOD(Ar16t16r_SetMode,		 setModeAr16t16r ),	    
	    DEVMETHOD(Ar16t16r_SetStart,     setStartAr16t16r),
        DEVMETHOD_END
};

/*
 * Prototypes
 */
LOCAL void Ar16t16rInstInit   (VXB_DEVICE_ID pInst);
LOCAL void Ar16t16rInstInit2  (VXB_DEVICE_ID pInst);
LOCAL void Ar16t16rInstConnect(VXB_DEVICE_ID pInst);
//LOCAL void MonNikInitDevice (VXB_DEVICE_ID pInst, MONNIK_UINT32 inst );


//! 
LOCAL struct drvBusFuncs Ar16t16rFuncs =
{
    Ar16t16rInstInit,     /* devInstanceInit */
    Ar16t16rInstInit2,    /* devInstanceInit2 */
    Ar16t16rInstConnect   /* devConnect */
};
//!     PCI
LOCAL struct vxbPciID Ar16t16rPciDevIDList[] =
{
    /* { devID, vendID } */
    {   DEVICE_ID_AR16T16R, VENDOR_ID_AR16T16R}
};

//!    
LOCAL struct vxbPciRegister Ar16t16rDevReg =
{
{
    NULL,                       /* reserved for VxBus use */
    VXB_DEVID_DEVICE,           /* devID */
    VXB_BUSID_PCI,              /* busID = PCI */
    VXB_VER_4_0_0,              /* vxbVersion */
    "Ar16t16r",                 /* drvName */
    &Ar16t16rFuncs,             /* pDrvBusFuncs */
    &Ar16t16rDrv_methods[0],    /* pMethods */
    NULL,                       /* devProbe */
    NULL
},
1,
Ar16t16rPciDevIDList
};
#define  DELAY 500

void SleepMkc(int Mkc)
{

int Res;
struct timespec  rqtp,rmtp;
rqtp.tv_sec=0;
rqtp.tv_nsec=Mkc*10;
rmtp.tv_sec=0;
rmtp.tv_nsec=0;
Res=nanosleep
    (
     &rqtp, /* time to delay */
     NULL  /* premature wakeup (NULL=no result) */
    );
if (Res==ERROR)
   printf("\n Error in SleepMkc Mkc=%d",Mkc);

}
LOCAL int  LoadPlisHexFile(int BaseAdr,char *FileName)
{
uint8_t BufDat[MAX_FILESIZE_BYTE];
int Res,Handle,i,bit,j;
char Tmp[3];
char *Ptr;
//unsigned char tmp;

unsigned R;
union {
unsigned char Usc[2];
 unsigned short  Usi;
}
 Ub;

 Tmp[2]=(unsigned  char) 0;
Handle=open(FileName, O_RDONLY,0);
if    (Handle==-1)
 {
  printf("\n Error open file  PLISS  ->%s",FileName);
  return 1;
  }

//BufDat = new uint8_t [MAX_FILESIZE_BYTE];
sysOutLong (BaseAdr + XIL_RESET,0); 
SleepMkc(1000);//1mc
Res=read(Handle,(char *) BufDat, MAX_FILESIZE_BYTE);

close(Handle);
if (Res!=-1)
 {

 for(i=0;i<Res;i+=2)
   {

     Ub.Usc[1]=0;
    Tmp[0]=BufDat[i];
    Tmp[1]=BufDat[i+1];
    R =strtoul(Tmp, &Ptr, 16);
    Ub.Usc[0]=(unsigned char)R;

    for(bit=0;bit<8;bit++)
     {
     Ub.Usi<<=1;
     sysOutLong(BaseAdr + XIL_DAN,Ub.Usc[1]); 
    // SleepMkc(2);
    for(j=0;j<DELAY;j++);
     }
   }

}
 else
    printf("\n Error open file  PLISS  ->%s",FileName);



return 0;



}

uint32_t SwapChAddr(uint8_t ca)                                  
{
	uint32_t i;
	uint32_t res;
	uint32_t x01;
	//unsigned x0FF=0x00FF;
	res=0;
	x01=0x01;

	for(i=0; i != 8; i++)
	{ res=res<<1;
	if((ca>>i)&x01)
		res=res | x01;
	else res=res & (~x01);
	}
	return (res);
}

LOCAL uint32_t readLongWordAr16t16r(VXB_DEVICE_ID pInst,
									uint8_t io, /*  ((0)/(1)*/
									uint8_t nk, /*  0..15 ( 1*/
									uint8_t indexAddr)	/*  0..*/
{	
	int Lock=0;
	uint32_t baseAddr;
	uint32_t temp;
	const uint16_t* buf;
	
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
	
	if(io == 1)
		buf = ARBUF_TRANS;
	else
		buf = ARBUF_REC;
	
	//Lock=intLock();
	sysOutLong(baseAddr + ADR_RAM, buf[nk]|indexAddr);
	temp = sysInLong(baseAddr + DATA_RAM);
	//intUnlock(Lock);
	return   temp;
}
LOCAL void writeLongWordAr16t16r(VXB_DEVICE_ID pInst,
						 uint8_t io, /*  ((0)/(1)*/
						 uint8_t nk, /*  0..15 ( 1*/
						 uint8_t indexAddr,	/*  0..*/
						 uint32_t data)/* 32 */
{	
	int Lock=0;
	uint32_t baseAddr;	
	const uint16_t* buf;
	
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
	
	if(io == 1)
		buf = ARBUF_TRANS;
	else
		buf = ARBUF_REC;
	
	//Lock=intLock();
	sysOutLong(baseAddr + ADR_RAM, buf[nk]|indexAddr);
	sysOutLong(baseAddr + DATA_RAM, data);
	//intUnlock(Lock);	
}
//!  
LOCAL void flashFirmwareAr16t16r(VXB_DEVICE_ID pInst,
						 uint8_t* binData,		/*coverted data from HEX to BIN*/
						 uint32_t sizeBytes )	/*size of BIN data*/
{	
	uint8_t  bit,tmp;
	uint32_t i			   = 0;
	uint32_t baseAddr	   = 0;
	uint32_t baseAddrFlash = baseAddr + XIL_DAN;
	
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	
	baseAddr 	  = pDrvCtrl->config->pci.baseAddr;
	baseAddrFlash+= baseAddr;
	//printf("Writing\n");
	//!  
	sysOutWord (baseAddr + XIL_RESET,0);
	//!  1
	vxbDelay();
	
	if (binData!=NULL)
	{
		for(i=0;i<sizeBytes;i++)
	    {
			tmp = binData[i];
			for(bit=0;bit<8;bit++) //8   
			{				
				sysOutByte(baseAddrFlash,(tmp&0x80)>>7);
				tmp<<=1;
				vxbUsDelay(1);//2 
			}
	    }
	}
	//!  
	sysOutLong(baseAddr + EN_IRQ,0);//disable interrupt
	sysOutLong(baseAddr + RES_IRQ,0xffff);
}

LOCAL void loadFirmware(VXB_DEVICE_ID pInst)
{
	int fd;
	uint32_t bytesRead;
	//uint32_t baseAddr;
	LOCAL char buffer[MAX_FILESIZE_BYTE];
	
	//!       
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	
	//printf("try to read file from romfs\n");
	//pDrvCtrl->config->pci.baseAddr;
	fd = open(FIRMWARE_CFG_FILE_PATH, O_RDONLY,0);
	if (fd < 0) {
		logMsg(" failed to open firmware bin file\n ",0,0,0,0,0,0);		
		close(fd);
		//return (ERROR);
	}
	bytesRead = read(fd,(void*)buffer, MAX_FILESIZE_BYTE);
	if(bytesRead<0)
	{
		printf("loadFirmware(): can`t read data from buffer\n");
	}
	//printf("Bytes = %d\n ",bytesRead);
	close(fd);
	/*if (bytesRead != CONFIG_FILESIZE) {
		AR16T16R_DBG_MSG(0, "%s: failed to read config file. Errno: %x", mName, errno, 1, 1, 1, 1);
		return (ERROR);
	}*/
	
	//LoadPlisHexFile(0xdc01,)
	
	flashFirmwareAr16t16r(pInst,(uint8_t*)buffer,bytesRead);
}


LOCAL void setStartAr16t16r(VXB_DEVICE_ID pInst,
							uint8_t io,		/*  ((0)/(1)*/
							uint8_t nk, 	/* */
							uint8_t value)	/*start(1)/stop(0)*/
{
	uint8_t seek;
	uint32_t baseAddr;
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
	//Lock=intLock();	
	if(io)
		seek = REG_START_STOP_TR_RW;
	else
		seek = REG_EN_DIS_RC_RW;
		
	sysOutLong(baseAddr + MODE,seek|nk);
	baseAddr+=OPERATION;
	
	if(value == 1)
		sysOutLong(baseAddr,1);
    else
		sysInLong(baseAddr);	
//intUnlock(Lock);

}
LOCAL void setModeAr16t16r(VXB_DEVICE_ID pInst,
						   uint8_t io,		/*  ((0)/(1)*/
						   uint8_t nk, 		/* */
						   uint32_t regime,	 /* */
						   uint8_t numAddr,	 /*- */
						   uint8_t *listAddr)/* */
{
	uint8_t seek,i;	
	uint32_t baseAddr;
	TAR16T16RDEVICEINFO *pDrvCtrl = (TAR16T16RDEVICEINFO *)(pInst->pDrvCtrl);
	baseAddr = pDrvCtrl->config->pci.baseAddr;
		
	//!   
	for(i=0;i<numAddr;i++)
	{
		writeLongWordAr16t16r(pInst,io,nk,i,SwapChAddr(listAddr[i]));		
	}
	if(io)
		seek = REG_PARAM_TR_W;
	else
		seek = REG_PARAM_RC_W;
	sysOutLong(baseAddr + MODE, seek|nk);
	sysOutLong(baseAddr + OPERATION,regime);
}

//!     CDF 
void Ar16t16rPciRegister(void)
{
    //!  ,    
    LOCAL BOOL initCalled = FALSE;

    if (initCalled == FALSE)
    {
        vxbDevRegister((struct vxbDevRegInfo *)&Ar16t16rDevReg);
        initCalled = TRUE;
    }
    return;
}
LOCAL void Ar16t16rInstInit ( VXB_DEVICE_ID pInst )
{
	
    TAR16T16RDEVICEINFO *devInfo;
    //logMsg(" Init\n ",0,0,0,0,0,0);
    if(ar_device_count >= AR16T16R_MAX_DEVICES)
    {
        return;
    }
    pInst->pDrvCtrl = (TAR16T16RDEVICEINFO *)hwMemAlloc(sizeof(TAR16T16RDEVICEINFO));
    if(pInst->pDrvCtrl == NULL)
    {
        return;
    }
    bzero((char *)pInst->pDrvCtrl, sizeof(TAR16T16RDEVICEINFO));
    Ar16t16rInitDevice( pInst, ar_device_count );

    devInfo = pInst->pDrvCtrl;
//    if(devInfo->config == NULL)
//    {
//    	logMsg(" Bad\n ",0,0,0,0,0,0);
//        return;
//    }
    devInfo->unit     = ar_device_count;
    devInfo->devId    = pInst;
    devInfo->instance = 0;
    ar_device_count++;

    vxbNextUnitGet (pInst);
    return;
}

LOCAL void Ar16t16rInstInit2( VXB_DEVICE_ID pInst )
{
    return;
}

LOCAL void Ar16t16rInstConnect( VXB_DEVICE_ID pInst )
{
    TAR16T16RCONFIG     *cfg;
    TAR16T16RDEVICEINFO  *devInfo = (TAR16T16RDEVICEINFO *)pInst->pDrvCtrl;
    
    cfg  = devInfo->config;
    printf("Adapter Ar16t16r = %d, BaseAddr = %x\n ",devInfo->unit,(int)cfg->pci.baseAddr,0,0,0,0,0);
    if(cfg == NULL)
    {
    	//logMsg(" Cfg null\n ",0,0,0,0,0,0);
        return;
    }
    //loadFirmware(pInst);
    //LoadPlisHexFile(cfg->pci.baseAddr,FIRMWARE_CFG_FILE_PATH);


	
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
LOCAL void Ar16t16rInitDevice( VXB_DEVICE_ID pInst, UINT32 inst )
{
    UINT8         cInst[4];
    UINT16        device;       /* Temp for device value                 */
    UINT8         intNum;       /* Temp for interrupt value              */
    UINT32        devfn;        /* Temp for devfn value                  */

    UINT8         bus;          /* Temp for interrupt value              */

    TAR16T16RCONFIG  *cfg = NULL;       /*   Local pointer to config structure     */
    UINT8         devicerev;
    UINT32        baseAddr = 0;    
    /*UINT32    size;
    UINT32 flags;*/

    struct vxbPciDevice *pPciDev;

    TAR16T16RDEVICEINFO *pDrvCtrl = pInst->pDrvCtrl;

    cfg = (TAR16T16RCONFIG *) hwMemAlloc (sizeof (TAR16T16RCONFIG));
    if(cfg == NULL)
    {
        pDrvCtrl->config = NULL;
        return;
    }
    bzero((char *)cfg, sizeof(TAR16T16RCONFIG));

    cfg->size = sizeof(TAR16T16RCONFIG);

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


    /* Get the device ID, e.g. 0x5565 */
    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_DEVICE_ID, 2, device );
    cfg->pci.type = device;

    /* Get the device revision */
    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_REVISION, 1, devicerev);
//
    /* Get the interrupt number */
    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_DEV_INT_LINE, 1, intNum);
    cfg->pci.intNum = intNum;
    
    VXB_PCI_BUS_CFG_READ (pInst, PCI_CFG_BASE_ADDRESS_0, 4, baseAddr);
    cfg->pci.baseAddr = (baseAddr>>1)<<1;
    
    pDrvCtrl->config = (TAR16T16RCONFIG *) cfg;

    return;
}








