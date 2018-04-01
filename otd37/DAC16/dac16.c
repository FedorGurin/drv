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



//#define DAC16_BASE_ADDRS "0x240"

//#define DAC16_CHN_PORT(baseAddr)         (baseAddr + 0x2) /* Номер канала */
//#define DAC16_VCODE_PORT(baseAddr)       (baseAddr)       /* Код напряжения (данные по адресу) */

//#define CODE_BY_VOLTAGE(v) (int) (((v + 10) * 8191.0)/20)
  
#define MAX_DAC16 8
struct dac16_drv_inst {
    uint16_t   *baseAddr[MAX_DAC16];
    uint8_t     numBaseAddr;
    SEM_ID      sem;
    DEV_HDR    *pDevHdr;
    struct dac16_drv_inst *next;
};

//static int dac16IosLoaded = 0;
//static char *config = NULL;
static struct dac16_drv_inst *dac16InstsLL=NULL;
static int dac16IosDrvInit = -1;

/* Forward declarations */
int dac16IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int dac16IosClose(struct dac16_drv_inst *pDrvInst);
int dac16IosWrite(struct dac16_drv_inst *pDrvInst, void *buffer, int nBytes);
int dac16IosIoctl(struct dac16_drv_inst *pDrvInst, int function, int arg);

/* Implmentation */




/*int dac16IosDrv(void)
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
}*/

void dac16IosDevAdd()
{
    struct dac16_drv_inst *tmp, *inst;
    DEV_HDR *pDevHdr;

    int i;
    char buffer[20];
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL) {
             return ERROR;
    }
    tmp = dac16InstsLL;
    inst = (struct dac16_drv_inst *) malloc(sizeof(struct dac16_drv_inst));
    if (inst == NULL) {
        free(pDevHdr);
        return ERROR;
    }
    memset( (void *)inst, 0, sizeof(struct dac16_drv_inst));

    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->pDevHdr = pDevHdr;   
    //inst->configured = 0;
        
    inst->next = NULL;
    if (dac16InstsLL == NULL)
        dac16InstsLL = inst;
    else {
        while (tmp->next != NULL) tmp = tmp->next;
        tmp->next = inst;
    }

    sprintf(buffer, "%s", DAC16_DEVPATH);
    iosDevAdd(pDevHdr, buffer, dac16IosDrvInit);
}

void dac16IosInit(void)
{
    char *me = "dac16IosInit()";

    if (dac16IosDrvInit!=-1)
        return;
    dac16IosDrvInit = iosDrvInstall(
            NULL,
            NULL,
            dac16IosOpen,
            dac16IosClose,
            NULL,
            dac16IosWrite,
            dac16IosIoctl);
    if (dac16IosDrvInit >0)
        dac16IosDevAdd();


//    STATUS s;
//    char *addrs = DAC16_BASE_ADDRS;
//	char *token;
//	int addr;
    
//    if (dac16IosLoaded)
//        return OK;
    
//    if ( (s = dac16IosDrv()) == ERROR)
//        return ERROR;
//    token = strtok(addrs, ",");
//    while (token != NULL) {
//    	  addr = strtol(token, NULL, 0);
//    	  if (addr)
//            dac16IosDevAdd(addr);
//        else
//            printf("%s: bad address: %s. Ignoring.\n", me, token);
//        token = strtok(NULL, ",");
//    }
//    return OK;
}

int dac16IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
    char *mName = "dac16IosOpen()";
    struct dac16_drv_inst *tmp;
    
    if (remainder != NULL && *remainder != '\0') {
        logMsg("dac16in reminder\n",
               0, 0, 0, 0, 0, 0);
        errnoSet(ENODEV);
           return ERROR;
       }
    tmp = dac16InstsLL;
    while (tmp != NULL && tmp->pDevHdr != pDevHdr) tmp = tmp->next;
    if (tmp == NULL) {
        errnoSet(ENOENT);
        return ERROR;
    }      
    return (int) tmp;
}

int dac16IosClose(struct dac16_drv_inst *pDrvInst)
{
    return OK;
}

int dac16IosWrite(struct dac16_drv_inst *pInst, void *buffer, int nBytes)
{
    dac16_write_cmd *writeCmd;
    UINT16 period, i, vcode, selCount;
    float Vcurr;
    
    if(pInst->numBaseAddr == 0)
    {
        errnoSet(ENODEV);
        return ERROR;
    }
    if (nBytes != sizeof(dac16_write_cmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    writeCmd = (dac16_write_cmd *) buffer;
    if (writeCmd == NULL) {
        errnoSet(EINVAL);
        return ERROR;
    }
//    if (writeCmd->channel >= DAC16_CHANNELS_PER_BOARD) {
//        errnoSet(EINVAL);
//        return ERROR;
//    }
    

    for(i = 0; i<pInst->numBaseAddr;i++)
    {
        //baseAddr
        /* выбираем канал */
        sysOutWord((ULONG)(pInst->baseAddr[i] + 0x2), writeCmd->channel[i]);
        /* записываем коды напряжения */

        sysOutWord((ULONG)(pInst->baseAddr[i] + 0x2), writeCmd->Vcode[i]); /* код */
    }
      
    return 0;
}

int dac16IosIoctl(struct dac16_drv_inst *pDrvInst, int function, int arg)
{
    if(function == REL48_IO_ADD_BASE_ADR)
    {
        pInst->baseAddr[pInst->numBaseAddr] = (uint32_t*)arg;
//        for (i = 0; i < REL48_REGS; i++)
//        {
//            pInst->currentState[i] = sysInWord((ULONG)pInst->baseAddr[pInst->numBaseAddr] + 2 * i);
//        }
        pInst->numBaseAddr++;
        return OK;
    }
    errnoSet(ENOSYS);
    return ERROR;
}
