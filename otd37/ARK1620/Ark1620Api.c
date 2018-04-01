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
#include "./include/Ark1620_defs.h"

//! признак того, что инициализация завершена
LOCAL uint8_t doneLibInitArk1620 = 0;

//! структура инстацирования
typedef struct TAR16T16R_INST
{	
	FUNCPTR flashFunc; 	  //!загрузка микрокода в адаптер
	FUNCPTR readLongWord; //!прочитать данные из IO MEM
	FUNCPTR writeLongWord;//!задать данные в IO MEM
	FUNCPTR setMode;      //!задать режим работы
	FUNCPTR setStart;     //!запуск канала
	VXB_DEVICE dev;  //!идентификатор устройства
	char nameDev;
	uint8_t devNum;
	
}AR16T16R_INST;

typedef struct
{
	AR16T16R_INST*   drvInst;	
}arincHandle_t;


//! все устройства AR16t16r
LOCAL AR16T16R_INST deviceAr16t16r[AR16T16R_MAX_DEVICES];

DEVMETHOD_DEF(Ar16t16r_FlashFirmware, "Write microcode to chip");
DEVMETHOD_DEF(Ar16t16r_ReadLongWord , "Read long word from channel adapter");
DEVMETHOD_DEF(Ar16t16r_WriteLongWord, "Write long word to channel adapter");
DEVMETHOD_DEF(Ar16t16r_SetMode, 	  "Set mode channel adapter");
DEVMETHOD_DEF(Ar16t16r_SetStart, 	  "Switch on/off channel adapter");

//! инициализации библиотеки работы с адаптерами Ar16t16r
//! здесь я должен пробежаться по всем этим элементам и создать объекты
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
//! функция нужна чтобы найти устройство
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
	//! если имена совпадают
	if(strcmp(devID->pName,devName)==0 )
	{
		deviceAr16t16r[devID->unitNumber].flashFunc 	= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_FlashFirmware));	
		deviceAr16t16r[devID->unitNumber].readLongWord 	= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_ReadLongWord));
		deviceAr16t16r[devID->unitNumber].writeLongWord = vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_WriteLongWord));
		deviceAr16t16r[devID->unitNumber].setMode 		= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_SetMode));
		deviceAr16t16r[devID->unitNumber].setStart 		= vxbDevMethodGet(devID, DEVMETHOD_CALL(Ar16t16r_SetStart));
		deviceAr16t16r[devID->unitNumber].dev           = *devID;
		deviceAr16t16r[devID->unitNumber].devNum        = devID->unitNumber;
		
	}
	return OK;
}

//! загрузка микрокода в платы
STATUS UploadMicroCodeAPI()
{
	int i=0;
	if(doneLibInitAr16t16r == 0)
	{
		//! инициализация не выполнена
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
		//! иницализация была уже выполнена
		return ERROR;
	}
	//! осуществляем поиск устройств
	vxbDevIterate(DeviceHelperAr16t16r,NULL, VXB_ITERATE_INSTANCES);
	doneLibInitAr16t16r = 1;
	printf("start downlaod\n");
	if(UploadMicroCodeAPI() == ERROR) printf("error in UploadMicroCodeAPI()\n");
    else     printf("loading finished\n");
	return OK;
}
STATUS checkNumChannelArinc(uint8_t numChannel, uint8_t *numA, uint8_t *numCh)
{
	uint8_t maxCh;
	if(doneLibInitAr16t16r == 0)
	{
		return ERROR;
	}
	//numCh = numChannel/16;
	 maxCh = (1<<AR16T16R_MAX_CHANNEL_POW_2);
	*numA  = numChannel>>AR16T16R_MAX_CHANNEL_POW_2;
	*numCh = numChannel - (*numA) * maxCh;
	if((*numA)>AR16T16R_MAX_DEVICES || (*numCh)>maxCh)
		return ERROR;
	return OK;
}
STATUS ReadLongWordArincAPI(uint8_t numChannel, /*номер канала сквозная нумерация 0..*/
						    uint8_t io, 		/*признак типа канала(входной(0)/выходной(1)*/						    
						    uint8_t addr)		/*8-ричный адрес*/
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
STATUS WriteLongWordArincAPI(uint8_t numChannel,/*номер канала сквозная нумерация 0..*/
							 uint8_t io, 		/*признак типа канала(входной(0)/выходной(1)*/							 
						     uint8_t addr,		/*8-ричный адрес*/
						     uint32_t data)		/*слово данных*/
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
//! задание режима приемного канала
STATUS SetModeRCArincAPI(uint8_t numChannel,/*номер канала сквозная нумерация 0..*/
						 uint8_t addr,
						 uint8_t freq,	   /* 00 - 50 кГц, 01 - 100 кГц, 10 - 12.5 кГц*/
						 uint8_t switchBuf,/* переключение буфферов (1 - разрешает)*/
						 uint8_t gotov)    /* признак разрешения/запрещения работы по "Запрос" 0- разрешает принимать данные без запроса*/
{
	uint8_t numA;
	uint8_t numCh;
	uint32_t regime;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
	(*deviceAr16t16r[numA].setMode)(&(deviceAr16t16r[numA].dev),0,numCh,regime);	
	return OK;	
}
//! задание режима канала передачи
STATUS SetModeTRArincAPI(uint8_t numChannel,		/* номер канала сквозная нумерация 0..*/
						 uint8_t numWord,   		/* кол-во слов передаваемых из буфера в канал*/
						 uint32_t single,			/* выдача 0 - циклическая, 1 - одинократно*/
						 uint32_t freq,				/* 00 - 50 кГц, 01 - 100 кГц, 10 - 12.5 кГц*/
						 uint32_t interval, 		/* пауза между передаваемым словами в тактах частоты*/
						 uint8_t switchBuf, 		/* переключение буферов (1 - разрешает)*/
						 uint8_t gotov 		)		/* признак разрешения/запрещения работы по сигналу "Запрос"*/
						 
{
	uint8_t numA;
	uint8_t numCh;
	uint32_t regime;
	if(checkNumChannelArinc(numChannel,&numA,&numCh) == ERROR)
		return ERROR;
		
	regime = numWord - 1;
	
	regime|= interval<<13;
	regime|= freq<<9;
	regime|= switchBuf<<11;
	regime|= gotov<<12;
	
	(*deviceAr16t16r[numA].setMode)(&(deviceAr16t16r[numA].dev),1,numCh,regime);
	return OK;
	
}

STATUS SetStartArincAPI(uint8_t numChannel,/*номер канала сквозная нумерация 0..*/
						uint8_t io,		   /*признак типа канала(входной(0)/выходной(1)*/						
						uint8_t value)	   /*запуск(1)/останов(0)*/
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








