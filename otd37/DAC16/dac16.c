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
#include "dac16.h"



#define DAC16_BASE_ADDRS "0x240"

#define DAC16_CHN_PORT(dev)         (dev->baseAddr + 0x2) /* Номер канала */
#define DAC16_VCODE_PORT(dev)       (dev->baseAddr)       /* Код напряжения (данные по адресу) */

#define CODE_BY_VOLTAGE(v) (int) (((v + 10) * 8191.0)/20)
  
struct dac16_drv_inst {
    uint16_t baseAddr;
    DEV_HDR *pDevHdr;
    SEM_ID sem;
    int configured;   
    struct dac16_drv_inst *next;
};

static int dac16IosLoaded = 0;
//static char *config = NULL;
static struct dac16_drv_inst *dac16Devs=NULL;
static int dac16IosDrvNum = -1;

/* Forward declarations */
int dac16IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int dac16IosClose(struct dac16_drv_inst *pDrvInst);
int dac16IosWrite(struct dac16_drv_inst *pDrvInst, void *buffer, int nBytes);
int dac16IosIoctl(struct dac16_drv_inst *pDrvInst, int function, int arg);

/* Implmentation */




int dac16IosDrv(void)
{
    if (dac16IosDrvNum > 0)
        return OK;
    dac16IosDrvNum = iosDrvInstall(
            NULL, 
            NULL, 
            dac16IosOpen, 
            dac16IosClose, 
            NULL, 
            dac16IosWrite, 
            dac16IosIoctl);
    if (dac16IosDrvNum < 0)
        return ERROR;
    return OK;
}

int dac16IosDevAdd(int addr)
{
	        
    static int devCount = 0;
    DEV_HDR *pDevHdr;
    static struct dac16_drv_inst *last = NULL;
    struct dac16_drv_inst *inst = NULL;
    int i;
    char buffer[20];
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL) {
   	            return ERROR;
   	        }
    inst = (struct dac16_drv_inst *) malloc(sizeof(struct dac16_drv_inst));
    if (inst == NULL) {
    	            free(pDevHdr);
    	            return ERROR;
    	        }
    
    inst->baseAddr = addr;
    inst->pDevHdr = pDevHdr;
    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->configured = 0;
        
    sprintf(buffer, "%s%d", DAC16_DEVPATH, devCount++);
    iosDevAdd(pDevHdr, buffer, dac16IosDrvNum);
    if (last == NULL) {
    	dac16Devs = last = inst;
    	        } else {
    	            last->next = inst;
    	            last = inst;
    	        }
    	        return OK;
    return OK;
    
    
   
}

int dac16IosInit(void)
{
    char *me = "dac16IosInit()";
    STATUS s;
    char *addrs = DAC16_BASE_ADDRS;
	char *token;
	int addr;
    
    if (dac16IosLoaded)
        return OK;
    
    if ( (s = dac16IosDrv()) == ERROR)
        return ERROR;
    token = strtok(addrs, ",");
    while (token != NULL) {
    	  addr = strtol(token, NULL, 0);
    	  if (addr)
            dac16IosDevAdd(addr);
        else
            printf("%s: bad address: %s. Ignoring.\n", me, token);
        token = strtok(NULL, ",");
    }
    return OK;  
}

int dac16IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
    char *mName = "dac16IosOpen()";
    struct dac16_drv_inst *tmp = dac16Devs;    
    
    if (remainder != NULL && *remainder != '\0') {
    	logMsg("dac16in reminder\n", 
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
    return (int) tmp;
}

int dac16IosClose(struct dac16_drv_inst *pDrvInst)
{
    return 0;
}

int dac16IosWrite(struct dac16_drv_inst *pDrvInst, void *buffer, int nBytes)
{
    dac16_write_cmd *writeCmd;
    UINT16 period, i, vcode, selCount;
    float Vcurr;
    
    if (nBytes != sizeof(dac16_write_cmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    writeCmd = (dac16_write_cmd *) buffer;
    if (writeCmd == NULL) {
        errnoSet(EINVAL);
        return ERROR;
    }
    if (writeCmd->channel >= DAC16_CHANNELS_PER_BOARD) {
        errnoSet(EINVAL);
        return ERROR;
    }
    
    Vcurr = writeCmd->Vs;
    /* выбираем канал */
    sysOutWord(DAC16_CHN_PORT(pDrvInst), writeCmd->channel);
    /* записываем коды напряжения */   
    vcode = CODE_BY_VOLTAGE(Vcurr);    
    sysOutWord(DAC16_VCODE_PORT(pDrvInst), vcode); /* код */       
      
    return 0;
}

int dac16IosIoctl(struct dac16_drv_inst *pDrvInst, int function, int arg)
{
    return 0;
}
