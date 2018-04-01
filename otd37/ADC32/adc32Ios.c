#include <vxWorks.h>
#include <errnoLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vxbTimerLib.h>
#include "adc32.h"

#define ADC32_BASE_ADDRS "0x230"
#define ADC32_CFG_PATH  "/romfs/ADC32M_v1.BIT"
#define ADC32_CFG_SIZE  42167


static int adc32DrvNum = -1;

struct adc32DrvInst {
    uint16_t baseAddr;
    SEM_ID sem;
    DEV_HDR *pDevHdr;
    int currentPeriod;
    int configured;
    struct adc32DrvInst *next;
};

static struct adc32DrvInst *adc32InstsLL = NULL; /* Linked list of instances */

/* forward declarations */
int iosAdc32Open(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int iosAdc32Close(struct adc32DrvInst *pInst);
int iosAdc32Read(struct adc32DrvInst *pInst, void *buffer, int nBytes);
int iosAdc32Ioctl(struct adc32DrvInst *pInst, int function, int arg);

int adc32GetConfig(uint8_t **);

int iosAdc32DevInit(struct adc32DrvInst *pDrvInst)
{
    uint16_t addr = pDrvInst->baseAddr;   
    uint8_t *config = NULL;
    int configSize, i, shift;  
    configSize = adc32GetConfig(&config);
    if (configSize < 0) {
        return ERROR;
    }
    sysOutByte(ADC32_INIT_PORT(addr), 0);
    vxbDelay();
    for (i = 0; i < configSize; i++)
        for (shift = 7; shift >= 0; shift--)
            sysOutByte(ADC32_CONFIG_PORT(addr), config[i] >> shift);
    sysOutWord(ADC32_PERIOD_PORT(addr), ADC32_DEFAULT_PERIOD);
    sysOutWord(ADC32_ENABLE_PORT(addr), 1);
    pDrvInst->currentPeriod = ADC32_DEFAULT_PERIOD;
    pDrvInst->configured = 1;
    return OK;
}

int iosAdc32AddDev(int baseAddr)
{
    static struct adc32DrvInst *last = NULL;
    static int devCount = 0;
    
    struct adc32DrvInst *inst = NULL;
    DEV_HDR *pDevHdr;
    char buffer[20];
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL) {
        return ERROR;
    }
    inst = (struct adc32DrvInst *) malloc(sizeof(struct adc32DrvInst));
    if (inst == NULL) {
        free(pDevHdr);
        return ERROR;
    }
    inst->baseAddr = baseAddr;
    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->pDevHdr = pDevHdr;
    inst->configured = 0;
    
	sprintf(buffer, "%s%d", ADC32_DEVPATH, devCount++);
	iosDevAdd(pDevHdr, buffer, adc32DrvNum);
    if (last == NULL) {
        adc32InstsLL = last = inst;
    } else {
        last->next = inst;
        last = inst;
    }
    return OK;
}

int iosAdc32AddDevs(void)
{
    char *me = "iosAdc32AddDevs()";
    char *token, *addrs = ADC32_BASE_ADDRS;
    int addr;
    token = strtok(addrs, ",");
    while (token != NULL) {
        addr = strtol(token, NULL, 0);
        if (addr)
            iosAdc32AddDev(addr);
        else
            logMsg("%s: bad address %s. Skipping...", (int)me, (int)token, 0, 0, 0, 0);
        token = strtok(NULL, ",");
    }
    return OK;
}

int iosAdc32Init(void)
{
    adc32DrvNum = iosDrvInstall(NULL, NULL, iosAdc32Open, iosAdc32Close, iosAdc32Read, NULL, iosAdc32Ioctl);
    if (adc32DrvNum < 0)
        return ERROR;
    return iosAdc32AddDevs();
}

int iosAdc32Open(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
	struct adc32DrvInst *tmp = adc32InstsLL;
	
    if (remainder != NULL && *remainder != '\0') {
    	logMsg("adc:in reminder\n", 
    	                        0, 0, 0, 0, 0, 0);
        errnoSet(ENODEV);
        return ERROR;
    }
    if (flags != O_RDONLY) {
        errnoSet(EACCES);
        return ERROR;
    }
    logMsg("adc:line 135\n", 
                           0, 0, 0, 0, 0, 0);
    while (tmp != NULL && tmp->pDevHdr != pDevHdr) tmp = tmp->next;
    if (tmp == NULL) {
        errnoSet(ENODEV);
        return ERROR;
    }
    logMsg("adc:line 142\n", 
                               0, 0, 0, 0, 0, 0);
    if (tmp->configured == 0 && iosAdc32DevInit(tmp) == ERROR) {
        errnoSet(-1);
        return ERROR;
    }
    logMsg("adc device burinig\n", 
                        0, 0, 0, 0, 0, 0);
        
    return (int) tmp;
}

int iosAdc32Close(struct adc32DrvInst *pInst)
{
    return OK;
}

int iosAdc32Read(struct adc32DrvInst *pInst, void *buffer, int nBytes)
{
	struct adc32ReadCmd *cmd;
	int i, channelCount;
	uint16_t addr;
	
    if (nBytes != sizeof(struct adc32ReadCmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    cmd = (struct adc32ReadCmd *) buffer;
    if (cmd->firstChannel < 0 || cmd->firstChannel > ADC32_CHANNELS_PER_BOARD) {
        errnoSet(EINVAL);
        return ERROR;
    }
    channelCount = cmd->channelCount;
    if (channelCount > ADC32_CHANNELS_PER_BOARD - cmd->firstChannel) {
        channelCount = ADC32_CHANNELS_PER_BOARD - cmd->firstChannel;
    }
    addr = pInst->baseAddr;
    sysOutWord(ADC32_CHANNEL_PORT(addr), cmd->firstChannel);
    for (i = 0; i < channelCount; i++)
        *(cmd->buffer + i) = sysInWord(ADC32_DATA_PORT(addr));
    return channelCount;
}

/* ioctl routines */
void ioctlSetPeriod(struct adc32DrvInst *pInst, struct adc32SetPeriodCmd *cmd)
{
	pInst->currentPeriod = cmd->period;
	sysOutWord(ADC32_PERIOD_PORT(pInst->baseAddr), cmd->period);
}

void ioctlGetPeriod(struct adc32DrvInst *pInst, struct adc32GetPeriodCmd *cmd)
{
	*(cmd->buffer) = pInst->currentPeriod;
}

int iosAdc32Ioctl(struct adc32DrvInst *pInst, int function, int arg)
{
    switch (function) {
    case IOCADCSETPERIOD: {
        ioctlSetPeriod(pInst, (struct adc32SetPeriodCmd *) arg);
        break;
    }
    case IOCADCGETPERIOD: {
        ioctlGetPeriod(pInst, (struct adc32GetPeriodCmd *) arg);
        break;
    }
    default:
        errnoSet(ENOSYS);
        return ERROR;
    }
    return OK;
}


int adc32GetConfig(uint8_t **buffer)
{
    static char *me = "adc32GetConfig()";
    static uint8_t *config = NULL;
    static int configSize = 0;
    static int errorReported = 0;
    
    FILE *f;
    int bytesRead = 0;
     
    if (config == NULL) {
        config = (uint8_t *) malloc(sizeof(uint8_t) * ADC32_CFG_SIZE);
        if (config == NULL) {
            return ERROR;
        }
        f = fopen(ADC32_CFG_PATH, "rb");
        if (f == NULL) {
            free(config);
            config = NULL;
            return ERROR;
        }
        while (configSize != ADC32_CFG_SIZE) {
            bytesRead = fread(config + configSize, sizeof(char), ADC32_CFG_SIZE, f);
            if (!bytesRead) {
                /* Determine, what error occured, only if it wasn't reported yet */
                if (!errorReported) {
                    if (feof(f))
                        logMsg("%s: encountered end of error while loading configuration. Total bytes read: %d, expected %d.", 
                                (int)me, (int)configSize, (int)ADC32_CFG_SIZE, 0, 0, 0);
                    else if (ferror(f))
                        logMsg("%s: some error occured while loading confugration. Please, check that file exists and accesible at %s",
                                (int)me, (int)ADC32_CFG_PATH, 0, 0, 0, 0);
                    errorReported = !errorReported;
                }
                free(config);
                config = NULL;
                configSize = 0;
                fclose(f);
            }
            configSize += bytesRead;
        }
        
        fclose(f);
    }
    *buffer = config;
    return configSize;
}
