#include <vxWorks.h>
#include <intLib.h>
#include <stdio.h>
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
#include "./include/Ar16t16r_defs.h"
#include "./include/Ar16t16rApi.h"

//!  ,   
LOCAL uint8_t doneLibInitAr16t16r = 0;

//!  
typedef struct TAR16T16R_INST
{	
	FUNCPTR flashFunc; 	  //!   
	FUNCPTR readLongWord; //!   IO MEM
	FUNCPTR writeLongWord;//!   IO MEM
	FUNCPTR setMode;      //!  
	FUNCPTR setStart;     //! 
	VXB_DEVICE dev;  //! 
	char nameDev;
	uint8_t devNum;
	
}AR16T16R_INST;

typedef struct
{
	AR16T16R_INST*   drvInst;	
}arincHandle_t;


//!   AR16t16r
LOCAL AR16T16R_INST deviceAr16t16r[AR16T16R_MAX_DEVICES];
LOCAL int8_t numDeviceAr16t16r = -1;

DEVMETHOD_DEF(Ar16t16r_FlashFirmware, "Write microcode to chip");
DEVMETHOD_DEF(Ar16t16r_ReadLongWord , "Read long word from channel adapter");
DEVMETHOD_DEF(Ar16t16r_WriteLongWord, "Write long word to channel adapter");
DEVMETHOD_DEF(Ar16t16r_SetMode, 	  "Set mode channel adapter");
DEVMETHOD_DEF(Ar16t16r_SetStart, 	  "Switch on/off channel adapter");

//!    
IMPORT STATUS SetModeRCArincAPI(uint8_t numChannel,/*    0..*/
						 uint8_t numAddr,   /* -    */
						 uint8_t *listAddr,	/*     */						 
						 uint8_t freq,	   /* 00 - 50 , 01 - 100 , 10 - 12.5 */
						 uint8_t switchBuf,/*   (1 - )*/
						 uint8_t gotov);    /*  /   "" 0-     */

//!      Ar16t16r
//!           
STATUS LibInitAr16T16R()
{
	uint8_t devCnt = 0;
	for(devCnt = 0; devCnt < AR16T16R_MAX_DEVICES; devCnt++)
	{
		//deviceAr16t16r[devCnt].dev 	 	= NULL;
		deviceAr16t16r[devCnt].flashFunc 	= NULL;
		deviceAr16t16r[devCnt].readLongWord = NULL;
		deviceAr16t16r[devCnt].writeLongWord= NULL;
		deviceAr16t16r[devCnt].setMode 		= NULL;
		deviceAr16t16r[devCnt].setStart 	= NULL;
	}
	return OK;
}
//!     
LOCAL STATUS DeviceHelperAr16t16r ( VXB_DEVICE_ID devID, void * dummy)
{
	int defCnt = 0;
	char *devName = "Ar16t16r";

	/* check for the validity of the function parameters */
	if (devID == NULL)
	{
		return (ERROR);
	}
	
	//printf("npName=%s\n",devID->pName);
	//!   
	if(strcmp(devID->pName,devName)==0 )
	{
		deviceAr16t16r[devID->unitNumber].flashFunc 	= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_FlashFirmware));	
		deviceAr16t16r[devID->unitNumber].readLongWord 	= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_ReadLongWord));
		deviceAr16t16r[devID->unitNumber].writeLongWord = vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_WriteLongWord));
		deviceAr16t16r[devID->unitNumber].setMode 		= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_SetMode));
		deviceAr16t16r[devID->unitNumber].setStart 		= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_SetStart));
		deviceAr16t16r[devID->unitNumber].dev           = *devID;
		deviceAr16t16r[devID->unitNumber].devNum        = devID->unitNumber;
		numDeviceAr16t16r = devID->unitNumber;
		
	}
	return OK;
}

//!    
STATUS UploadMicroCodeAPI()
{
	int i=0;
	if(doneLibInitAr16t16r == 0)
	{
		//!   
		return ERROR;
	}
	for(i=0;i<AR16T16R_MAX_DEVICES;i++)
	{
		if(deviceAr16t16r[i].flashFunc!= NULL)
		{
			(*deviceAr16t16r[i].flashFunc)(&(deviceAr16t16r[i].dev));
		}
	}
	return OK;
}
STATUS OpenArincAPI()
{
	if(doneLibInitAr16t16r == 1)
	{
		//!    
		return OK;
	}
	//!   
	vxbDevIterate(DeviceHelperAr16t16r,NULL, VXB_ITERATE_INSTANCES);
	doneLibInitAr16t16r = 1;
	
	if(UploadMicroCodeAPI() == ERROR)
		printf("OpenArincAPI() : error in UploadMicroCodeAPI() function\n");
   
	return OK;
}
//! -  ,         
STATUS checkNumChannelArinc(uint8_t numChannel, uint8_t *numA, uint8_t *numCh)
{
	LOCAL uint8_t maxCh;
	LOCAL uint8_t codeRes=0;
	if(doneLibInitAr16t16r == 0)
	{
		return ERROR;
	}
	//numCh = numChannel/16;
	 maxCh = (1<<AR16T16R_MAX_CHANNEL_POW_2);
	*numA  = (numChannel>>AR16T16R_MAX_CHANNEL_POW_2);
	*numCh = numChannel - (*numA) * maxCh;
	*numA  = numDeviceAr16t16r - *numA;
	//printf("device=%d\n",numDeviceAr16t16r);
	if((*numA)>AR16T16R_MAX_DEVICES || (*numCh)>maxCh  || (*numA)>numDeviceAr16t16r)
	{
		printf("Drv AR16T16: Devices not avalible\n");
		codeRes = 1;
		return OK;
	}
	return OK;
}
uint16_t numChannelArinc()
{
	return ((numDeviceAr16t16r + 1) * (1<<AR16T16R_MAX_CHANNEL_POW_2)); 
}
STATUS ReadLongWordArincAPI(uint8_t numChannel, /*    0..*/
						    uint8_t io, 		/*  ((0)/(1)*/						    
						    uint8_t addr)		/*8- */
{
	uint8_t numA;
	uint8_t numCh;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
	if(deviceAr16t16r[numA].readLongWord == NULL)
	{
		return ERROR;
	}
	(*deviceAr16t16r[numA].readLongWord)(&(deviceAr16t16r[numA].dev),io,numCh,addr);
	return OK;
}
STATUS WriteLongWordArincAPI(uint8_t numChannel,/*    0..*/
							 uint8_t io, 		/*  ((0)/(1)*/							 
						     uint8_t addr,		/*8- */
						     uint32_t data)		/* */
{
	uint8_t numA;
	uint8_t numCh;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
	if(deviceAr16t16r[numA].writeLongWord == NULL)
	{
		return ERROR;
	}
	(*deviceAr16t16r[numA].writeLongWord)(&(deviceAr16t16r[numA].dev),io,numCh,addr,data);
	return OK;	
	
}
//!    
STATUS SetModeRCArincAPI(uint8_t numChannel,/*    0..*/
						 uint8_t numAddr,   /* -    */
						 uint8_t *listAddr,	/*     */						 
						 uint8_t freq,	   /* 00 - 50 , 01 - 100 , 10 - 12.5 */
						 uint8_t switchBuf,/*   (1 - )*/
						 uint8_t gotov)    /*  /   "" 0-     */
{
	uint8_t numA;
	uint8_t numCh;
	uint32_t regime=0;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
	//!    
	regime|=freq<<9; 
	regime|=switchBuf<<11;
	regime|=gotov<<12;
	if(deviceAr16t16r[numA].setMode == NULL)
	{
			return ERROR;
	}
	(*deviceAr16t16r[numA].setMode)(&(deviceAr16t16r[numA].dev),0,numCh,regime,numAddr,listAddr);	
	return OK;	
}
//!    
STATUS SetModeTRArincAPI(uint8_t numChannel,		/*     0..*/
						 uint8_t  numAddr,   		/* -    */
						 uint8_t *listAddr,			/*     */
						 uint32_t single,			/*  0 - , 1 - */
						 uint32_t freq,				/* 00 - 50 , 01 - 100 , 10 - 12.5 */
						 uint32_t interval, 		/*       ( 3)*/
						 uint8_t switchBuf, 		/*   (1 - )*/
						 uint8_t gotov 		)		/*  /    ""*/
						 
{
	uint8_t numA;
	uint8_t numCh;
	uint32_t regime;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
		
	regime = numAddr - 1;	
	regime|= single<<8;
	regime|= freq<<9;
	regime|= switchBuf<<11;
	regime|= gotov<<12;
	regime|= interval<<13;
	
	if(deviceAr16t16r[numA].setMode == NULL)
	{
		return ERROR;
	}
	
	(*deviceAr16t16r[numA].setMode)(&(deviceAr16t16r[numA].dev),1,numCh,regime,numAddr,listAddr);
	return OK;
	
}

STATUS SetStartArincAPI(uint8_t numChannel,/*    0..*/
						uint8_t io,		   /*  ((0)/(1)*/						
						uint8_t value)	   /*(1)/(0)*/
{
	uint8_t numA;
	uint8_t numCh;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
	if(deviceAr16t16r[numA].setStart == NULL)
	{
		return ERROR;
	}
	(*deviceAr16t16r[numA].setStart)(&(deviceAr16t16r[numA].dev),io,numCh,value);
	return OK;
	
}

///!    
void testStartChArinc50()
{
	uint8_t listAddr[4]={0220,0160,0111,0100};
	printf("testStartCh");
	if(WriteLongWordArincAPI(0,1,0220,0xF0FF)==ERROR)
		printf("Drv fail WriteLongWordArincAPI\n");
	if(WriteLongWordArincAPI(0,1,0160,0xF0FF)==ERROR)
		printf("fail WriteLongWordArincAPI\n");
	if(WriteLongWordArincAPI(0,1,0111,0xF0FF)==ERROR)
		printf("fail WriteLongWordArincAPI\n");		
	if(SetModeTRArincAPI(0,4,listAddr,0,0,2,0,0) == ERROR)
		printf("fail SetModeTRArincAPI\n");	
}
void testStartChArinc100()
{
	uint8_t listAddr[4]={0220,0160,0111,0100};
	if(SetModeTRArincAPI(0,4,listAddr,0,1,10,0,0) == ERROR)
		printf("fail SetModeTRArincAPI\n");	
}
void testStartChArinc12()
{
	uint8_t listAddr[3]={0220,0160,0111};
	if(SetModeTRArincAPI(0,3,listAddr,0,2,3,0,0) == ERROR)
		printf("fail SetModeTRArincAPI\n");	
}
void testStartChArincInter10()
{
	uint8_t listAddr[3]={0220,0160,0111};
	if(SetModeTRArincAPI(0,3,listAddr,0,2,10,0,0) == ERROR)
		printf("fail SetModeTRArincAPI\n");	
}
void testSwitchOnChArinc()
{
	printf("testSwitchOn");
	if(SetStartArincAPI(0,1,1) == ERROR)
		printf("fail SetStartArincAPI\n");	
}
void testSwitchOffChArinc()
{
	printf("testSwitchOn");
	if(SetStartArincAPI(0,1,1) == ERROR)	
		printf("fail SetStartArincAPI\n");

}






