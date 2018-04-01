#include <vxWorks.h>
#include <iosLib.h>
#include <sysLib.h>
#include <errnoLib.h>
#include <semLib.h>
#include <logLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <vxbTimerLib.h>
#include "dacwav.h"

#define DACWAV_API_DBG
#define DACWAV_CFG_FILE_SIZE 131027
#define DACWAV_CFG_FILE_PATH "/romfs/DAC_WAV_F.BIT"
#define DACWAV_BASE_ADDRS "0x280"
//#define DACWAV_CHANNELS_PER_BOARD 32
#define DACWAV_DEFAULT_PERIOD 100

/*#ifdef DACWAV_API_DBG
    static int dbgLevel = 0;
    #define DACWAV_API_DBG_LOG(level,fmt,a,b,c,d,e,f) \
        if (dbgLevel >= level) logMsg(fmt, a, b, c, d, e, f)
#else
    #define DACWAV_API_DBG_LOG(level,fmt,a,b,c,d,e,f)
#endif*/

#define DACWAV_INIT_PORT(dev)       (dev->baseAddr + 0x0) /* Сброс устройства */
#define DACWAV_CFG_PORT(dev)        (dev->baseAddr + 0x2) /* Конфигурация */
#define DACWAV_CHN_PORT(dev)        (dev->baseAddr + 0x4) /* Номер канала */
#define DACWAV_ADR_PORT(dev)        (dev->baseAddr + 0x6) /* Адрес во внутренней памяти (раздельные ап для каналов) */
#define DACWAV_VCODE_PORT(dev)      (dev->baseAddr + 0x8) /* Код напряжения (данные по адресу) */
#define DACWAV_SEL_PRD_PORT(dev)    (dev->baseAddr + 0xA) /* Период канала */
#define DACWAV_SEL_QNT_PORT(dev)    (dev->baseAddr + 0xC) /* Частота выборки */
#define DACWAV_CHN_WRK_PORT(dev)    (dev->baseAddr + 0xE) /*  */

#define CODE_BY_VOLTAGE(v) (int) (((v + 10) * 8192.0)/20)
  
struct dacwav_drv_inst {
    uint16_t baseAddr;
    DEV_HDR *pDevHdr;
    SEM_ID sem;
    int configured;
    unsigned short *periods;
    struct dacwav_drv_inst *next;
};

static int dacwavIosLoaded = 0;
//static char *config = NULL;
static struct dacwav_drv_inst *dacwavDevs=NULL;
static int dacwavIosDrvNum = -1;

/* Forward declarations */
int dacwavIosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int dacwavIosClose(struct dacwav_drv_inst *pDrvInst);
int dacwavIosWrite(struct dacwav_drv_inst *pDrvInst, void *buffer, int nBytes);
int dacwavIosIoctl(struct dacwav_drv_inst *pDrvInst, int function, int arg);

/* Implmentation */

int loadConfig(uint8_t **buffer) {
	
	
	 static char *me = "adc32GetConfig()";
	    static uint8_t *config = NULL;
	    static int configSize = 0;
	    static int errorReported = 0;
	    
	    FILE *f;
	    int bytesRead = 0;
	     
	    if (config == NULL) {
	        config = (uint8_t *) malloc(sizeof(uint8_t) * DACWAV_CFG_FILE_SIZE);
	        if (config == NULL) {
	            return ERROR;
	        }
	        f = fopen(DACWAV_CFG_FILE_PATH, "rb");
	        if (f == NULL) {
	            free(config);
	            config = NULL;
	            return ERROR;
	        }
	        while (configSize != DACWAV_CFG_FILE_SIZE) {
	            bytesRead = fread(config + configSize, sizeof(char), DACWAV_CFG_FILE_SIZE, f);
	            if (!bytesRead) {
	                /* Determine, what error occured, only if it wasn't reported yet */
	                if (!errorReported) {
	                    if (feof(f))
	                        logMsg("%s: encountered end of error while loading configuration. Total bytes read: %d, expected %d.", 
	                                (int)me, (int)configSize, (int)DACWAV_CFG_FILE_SIZE, 0, 0, 0);
	                    else if (ferror(f))
	                        logMsg("%s: some error occured while loading confugration. Please, check that file exists and accesible at %s",
	                                (int)me, (int)DACWAV_CFG_FILE_PATH, 0, 0, 0, 0);
	                    errorReported = !errorReported;
	                }
	                free(config);
	                config = NULL;
	                configSize = 0;
	                fclose(f);
	                //return ERROR;
	            }
	            configSize += bytesRead;
	        }
	        
	        fclose(f);
	    }
	    *buffer = config;
	    return configSize;
	   
	    
//    char *mName = "loadConfig()";
//    size_t len;
//    FILE *cfgFile;
//    
//    if (config != NULL)
//        return OK;
//    cfgFile = fopen(DACWAV_CFG_FILE_PATH, "rb");
//    config = malloc(sizeof(char) * DACWAV_CFG_FILE_SIZE);
//    fgets(config, DACWAV_CFG_FILE_SIZE + 1, cfgFile);
//    fclose(cfgFile);
//    if ( (len = strlen(config)) != DACWAV_CFG_FILE_SIZE) {
//        logMsg("%s: read %d bytes from config file, but expected %d. Please, fix configuration file and relad!\n", 
//                (int)mName, len, DACWAV_CFG_FILE_SIZE, 0, 0, 0);
//        free(config);
//        config = NULL;
//        return ERROR;
//    }
//    return OK;
}

int initDev(struct dacwav_drv_inst *dev) 
{
    int i=0, shift;
    uint8_t *config = NULL;
    int configSize;
    
    logMsg("dacvaw loadingConfig\n", 
                        0, 0, 0, 0, 0, 0);
    configSize = loadConfig(&config);
    if (configSize < 0) {
           return ERROR;
       }
    logMsg("dacvaw start device burinig\n", 
                            0, 0, 0, 0, 0, 0);
    sysOutWord(DACWAV_INIT_PORT(dev), 0x0);
    vxbDelay();
    
    for (i = 0; i < DACWAV_CFG_FILE_SIZE; i++)
    {
        for (shift = 7; shift >= 0; shift--)
        {           
            sysOutWord(DACWAV_CFG_PORT(dev), config[i] >> shift);         
        }
    }
    
    sysOutWord(DACWAV_CHN_PORT(dev), 0x1 << 4);
    sysOutWord(DACWAV_CHN_PORT(dev), 0x0 << 4);
    for (i = 0; i < DACWAV_CHANNELS_PER_BOARD; i++) {
        sysOutWord(DACWAV_SEL_PRD_PORT(dev), 0);
        sysOutWord(DACWAV_SEL_QNT_PORT(dev), 0);
    }
    
    dev->configured = 1;
    return OK;
}

int dacwavIosDrv(void)
{
    if (dacwavIosDrvNum > 0)
        return OK;
    dacwavIosDrvNum = iosDrvInstall(
            NULL, 
            NULL, 
            dacwavIosOpen, 
            dacwavIosClose, 
            NULL, 
            dacwavIosWrite, 
            dacwavIosIoctl);
    if (dacwavIosDrvNum < 0)
        return ERROR;
    return OK;
}

int dacwavIosDevAdd(int addr)
{
	        
    static int devCount = 0;
    DEV_HDR *pDevHdr;
    static struct dacwav_drv_inst *last = NULL;
    struct dacwav_drv_inst *inst = NULL;
    int i;
    char buffer[20];
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL) {
   	            return ERROR;
   	        }
    inst = (struct dacwav_drv_inst *) malloc(sizeof(struct dacwav_drv_inst));
    if (inst == NULL) {
    	            free(pDevHdr);
    	            return ERROR;
    	        }
    
    inst->baseAddr = addr;
    inst->pDevHdr = pDevHdr;
    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->configured = 0;
    inst->periods = (unsigned short *) malloc(sizeof(unsigned short) * DACWAV_CHANNELS_PER_BOARD);
            for (i = 0; i < DACWAV_CHANNELS_PER_BOARD; i++)
                inst->periods[i] = DACWAV_DEFAULT_PERIOD;
           // inst->next = NULL;
            
    /*dev->periods = (unsigned short *) malloc(sizeof(unsigned short) * DACWAV_CHANNELS_PER_BOARD);
    for (i = 0; i < DACWAV_CHANNELS_PER_BOARD; i++)
        dev->periods[i] = DACWAV_DEFAULT_PERIOD;
    dev->next = NULL;
    initDev(dev);
    
    if (dacwavDevs == NULL)
        dacwavDevs = dev;
    else {
        while (tmp->next != NULL) tmp = tmp->next;
        tmp->next = dev;
    }*/
    
    sprintf(buffer, "%s%d", DACWAV_DEVPATH, devCount++);
    iosDevAdd(pDevHdr, buffer, dacwavIosDrvNum);
    if (last == NULL) {
    	dacwavDevs = last = inst;
    	        } else {
    	            last->next = inst;
    	            last = inst;
    	        }
    	        return OK;
    return OK;
    
    
   
}

int dacwavIosInit(void)
{
    char *me = "dacwavIosInit()";
    STATUS s;
    char *addrs = DACWAV_BASE_ADDRS;
	char *token;
	int addr;
    
    if (dacwavIosLoaded)
        return OK;
    
    if ( (s = dacwavIosDrv()) == ERROR)
        return ERROR;
    token = strtok(addrs, ",");
    while (token != NULL) {
    	  addr = strtol(token, NULL, 0);
    	  if (addr)
            dacwavIosDevAdd(addr);
        else
            printf("%s: bad address: %s. Ignoring.\n", me, token);
        token = strtok(NULL, ",");
    }
    return OK;  
}

int dacwavIosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
    char *mName = "dacwavIosOpen()";
    struct dacwav_drv_inst *tmp = dacwavDevs;    
    
    if (remainder != NULL && *remainder != '\0') {
    	logMsg("dacwav:in reminder\n", 
    	    	                        0, 0, 0, 0, 0, 0);
           errnoSet(ENODEV);
           return ERROR;
       }
    /* 
     * TODO: is it ok to compare pointers with == here? 
     * Or should I compare what is under pointer?
     * */
    while (tmp != NULL && tmp->pDevHdr != pDevHdr) tmp = tmp->next;
    if (tmp == NULL) {
        errnoSet(ENOENT);
        return ERROR;
    }
    
    
    if (tmp->configured == 0 && initDev(tmp) == ERROR) {
            errnoSet(-1);
            return ERROR;
        }
    logMsg("dacvaw device burinig\n", 
                    0, 0, 0, 0, 0, 0);
    
    /*dev->periods = (unsigned short *) malloc(sizeof(unsigned short) * DACWAV_CHANNELS_PER_BOARD);
        for (i = 0; i < DACWAV_CHANNELS_PER_BOARD; i++)
            dev->periods[i] = DACWAV_DEFAULT_PERIOD;
        dev->next = NULL;
        initDev(dev);
        
        if (dacwavDevs == NULL)
            dacwavDevs = dev;
        else {
            while (tmp->next != NULL) tmp = tmp->next;
            tmp->next = dev;
        }*/
    
    return (int) tmp;
}

int dacwavIosClose(struct dacwav_drv_inst *pDrvInst)
{
    return 0;
}

int dacwavIosWrite(struct dacwav_drv_inst *pDrvInst, void *buffer, int nBytes)
{
    dacwav_write_cmd *writeCmd;
    UINT16 period, i, vcode, selCount;
    float Vcurr, dVi, dV;
    
    if (nBytes != sizeof(dacwav_write_cmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    writeCmd = (dacwav_write_cmd *) buffer;
    if (writeCmd == NULL) {
        errnoSet(EINVAL);
        return ERROR;
    }
    if (writeCmd->channel >= DACWAV_CHANNELS_PER_BOARD) {
        errnoSet(EINVAL);
        return ERROR;
    }
    period = pDrvInst->periods[writeCmd->channel];
    selCount = writeCmd->time / period;
    while (selCount > DACWAV_MAX_SELECTIONS) {
        period++;
        selCount = writeCmd->time / period; 
    }
    Vcurr = writeCmd->Vs;
    dV = writeCmd->dV;
    dVi = dV / selCount;
    /* выбираем канал */
    sysOutWord(DACWAV_CHN_PORT(pDrvInst), writeCmd->channel);
    /* записываем коды напряжения */
    for (i = 0; i < selCount; i++) {
        vcode = CODE_BY_VOLTAGE(Vcurr);
        sysOutWord(DACWAV_ADR_PORT(pDrvInst), i);       /* адрес */
        sysOutWord(DACWAV_VCODE_PORT(pDrvInst), vcode); /* код */
        Vcurr += dVi;
    }
    /* включаем передачу */
    sysOutWord(DACWAV_CHN_WRK_PORT(pDrvInst), 1 << writeCmd->channel);
    return 0;
}

int dacwavIosIoctl(struct dacwav_drv_inst *pDrvInst, int function, int arg)
{
    return 0;
}
