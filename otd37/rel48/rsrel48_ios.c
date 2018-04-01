#include <vxWorks.h>
#include <errnoLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rsrel48.h"

#define MAX_RSREL48 8
static int rsrel48DrvNum = -1;

struct rsrel48DrvInst {
    //! массив базовых адресов
    uint32_t *baseAddr[MAX_RSREL48]; 
    //! кол-во базовых адресов в массиве
    uint8_t numBaseAddr;
    SEM_ID sem;
    DEV_HDR *pDevHdr;
    struct rsrel48DrvInst *next;
};

static struct rsrel48DrvInst *rsrel48InstsLL = NULL; /* Linked list of instances */

/* forward declarations */
int rsrel48IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int rsrel48IosClose(struct rsrel48DrvInst *pInst);
int rsrel48IosRead(struct rsrel48DrvInst *pInst, void *buffer, int nBytes);
int rsrel48IosIoctl(struct rsrel48DrvInst *pInst, int function, int arg);


void rsrel48IosDevAdd()
{
    static int devCount = 0;
    char buffer[20];
    DEV_HDR *pDevHdr;
    struct rsrel48DrvInst *tmp = NULL, *inst = NULL;
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL) {
        return;
    }
    tmp = rsrel48InstsLL;
    inst = (struct rsrel48DrvInst *) malloc(sizeof(struct rsrel48DrvInst));
    if (inst == NULL) {
    	return;
    }
    memset( (void *)inst, 0, sizeof(struct rsrel48DrvInst));
    
    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->pDevHdr = pDevHdr;
    inst->numBaseAddr = 0;       
    
    inst->next = NULL;
    if (tmp != NULL) {
        while (tmp->next != NULL) {tmp = tmp->next;}
        tmp->next = inst;
    } else {
        rsrel48InstsLL = inst;
    }
    sprintf(buffer, "%s", RSREL48_DEVPATH);
    //! добавялем устройстов в список
    iosDevAdd(pDevHdr, buffer, rsrel48DrvNum);
}

void rsrel48IosInit(void)
{
    char *me = "rsrel48IosInit()";
    if (rsrel48DrvNum <= 0)
        rsrel48DrvNum = iosDrvInstall(NULL, NULL, rsrel48IosOpen, rsrel48IosClose, rsrel48IosRead, NULL, rsrel48IosIoctl); 
    if (rsrel48DrvNum > 0) {
    	rsrel48IosDevAdd();  
    }
}

int rsrel48IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
	struct rsrel48DrvInst *devInst = rsrel48InstsLL;
	
    if (remainder != NULL && *remainder != '\0') {
        errnoSet(ENODEV);
        return ERROR;
    }
    if (flags != O_RDONLY) {
        errnoSet(EACCES);
        return ERROR;
    }
    while (devInst != NULL && devInst->pDevHdr != pDevHdr) devInst = devInst->next;
    /* no device found */
    if (devInst == NULL) {
        errnoSet(ENODEV);
        return ERROR;
    }
    return (int) devInst;
}

int rsrel48IosClose(struct rsrel48DrvInst *pInst)
{
    return OK;
}

int rsrel48IosRead(struct rsrel48DrvInst *pInst, void *buffer, int nBytes)
{
	struct rsrel48ReadCmd *cmd = (struct rsrel48ReadCmd *) buffer;
	int bit, word, bitsRead = 0, i = 0, j = 0;
	int maxCh=0;
	    
	maxCh = REL48_CHANNELS_PER_BOARD * pInst->numBaseAddr;
		
	if(pInst->numBaseAddr == 0)
	{
	    errnoSet(ENODEV);
	    return ERROR;
	}
	if (nBytes != sizeof(struct rsrel48ReadCmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    if (cmd->startChannel >= maxCh) {
        errnoSet(ENOSPC);
        return ERROR;
    }
    
    for(i=0;i<pInst->numBaseAddr;i++)
    {
        for(j=0;j<2*REL48_REGS;j=j+2)
        {
            word = sysInWord((ULONG)pInst->baseAddr[i] + j);
            bit = 0;
            do
            {                
                cmd->values[bitsRead++] = word & 1;
                word>>=1;bit++;
            }while(REL48_WORD!=bit);
        }
    }
    return bitsRead;
}

int rsrel48IosIoctl(struct rsrel48DrvInst *pInst, int function, int arg)
{
    int i=0;
        if(function == REL48_IO_ADD_BASE_ADR)
        {
            pInst->baseAddr[pInst->numBaseAddr] = (uint32_t*)arg;
//            for (i = 0; i < REL48_REGS; i++) 
//            {
//                pInst->currentState[i] = sysInWord((ULONG)pInst->baseAddr[pInst->numBaseAddr] + 2 * i);
//            }       
            pInst->numBaseAddr++;
            return OK;
        }   

    errnoSet(ENOSYS);
    return ERROR;
}
