#include <vxWorks.h>
#include <errnoLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trrel48.h"



#define MAX_TRREL48 8
static int trrel48DrvInit = -1;

struct trrel48DrvInst {
    uint16_t *baseAddr[MAX_TRREL48];
    //! -    
    uint8_t numBaseAddr;
    SEM_ID sem;
    DEV_HDR *pDevHdr;
    struct trrel48DrvInst *next;
    int currentState[MAX_TRREL48 * REL48_REGS];
};

static struct trrel48DrvInst *trrel48InstsLL = NULL;

IMPORT void trrel48IosInit(void);
/* forward declarations */
int trrel48IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode);
int trrel48IosClose(struct trrel48DrvInst *pInst);
int trrel48IosWrite(struct trrel48DrvInst *pInst, void *buffer, int nBytes);
int trrel48IosIoctl(struct trrel48DrvInst *pInst, int function, int arg);


void trrel48IosDevAdd()
{
    struct trrel48DrvInst *tmp, *inst;
    DEV_HDR *pDevHdr;

    char buffer[20];
    
    pDevHdr = (DEV_HDR *) malloc(sizeof(DEV_HDR));
    if (pDevHdr == NULL){
            return ERROR;
    }
    tmp = trrel48InstsLL;
    inst = (struct trrel48DrvInst *) malloc(sizeof(struct trrel48DrvInst));
    if (inst == NULL) {
           free(pDevHdr);
           return ERROR;
    }
    memset( (void *)inst, 0, sizeof(struct trrel48DrvInst));    
           
    inst->sem = semMCreate(SEM_Q_FIFO);
    inst->pDevHdr = pDevHdr;
    inst->numBaseAddr = 0;
   
    inst->next = NULL;
    if (trrel48InstsLL == NULL)
        trrel48InstsLL = inst;
    else {
        while (tmp->next != NULL) tmp = tmp->next;
        tmp->next = inst;
    }
    sprintf(buffer, "%s", TRREL48_DEVPATH);
    iosDevAdd(pDevHdr, buffer, trrel48DrvInit);
}

void trrel48IosInit(void)
{
    char *me = "trrel48IosInit()";
    if(trrel48DrvInit!=-1)
        return;
    trrel48DrvInit = iosDrvInstall(NULL, NULL, trrel48IosOpen, trrel48IosClose, NULL, trrel48IosWrite, trrel48IosIoctl);
    if (trrel48DrvInit > 0)
        trrel48IosDevAdd();
}

int trrel48IosOpen(DEV_HDR *pDevHdr, char *remainder, int flags, int mode)
{
	struct trrel48DrvInst *tmp;
	
    if (remainder != NULL && *remainder != '\0') {
        errnoSet(ENODEV);
        return ERROR;
    }
    if (flags != O_RDONLY) {
        errnoSet(EACCES);
        return ERROR;
    }
    tmp = trrel48InstsLL;
    while (tmp != NULL && tmp->pDevHdr != pDevHdr) tmp = tmp->next;
    if (tmp == NULL) {
        errnoSet(ENODEV);
        return ERROR;
    }
    return (int) tmp;
}

int trrel48IosClose(struct trrel48DrvInst *pInst)
{
    return OK;
}
int trrel48IosRead(struct trrel48DrvInst *pInst, void *buffer, int nBytes)
{
	if(pInst->numBaseAddr == 0)
	{
		errnoSet(EINVAL);
		return ERROR;
	}	
	
    if (nBytes < sizeof(int) * MAX_TRREL48 * pInst->numBaseAddr) {
        errnoSet(EINVAL);
        return ERROR;
    }
    memcpy(buffer, (const void *) pInst->currentState, sizeof(int) * REL48_REGS * pInst->numBaseAddr);
    return sizeof(int) * REL48_REGS * pInst->numBaseAddr;
}

int trrel48IosWrite(struct trrel48DrvInst *pInst, void *buffer, int nBytes)
{
	struct trrel48WriteCmd *cmd;
	int maxCh=0,i=0,j=0,bit;
	int word;
	int bitsRead=0; 
	maxCh = REL48_CHANNELS_PER_BOARD * pInst->numBaseAddr;
	
	if(pInst->numBaseAddr == 0)
	{
		errnoSet(ENODEV);
		return ERROR;
	}		
	if (nBytes != sizeof(struct trrel48WriteCmd)) {
        errnoSet(EINVAL);
        return ERROR;
    }
    cmd = (struct trrel48WriteCmd *) buffer;
    if (cmd->startChannel >= maxCh) {
        errnoSet(ENOSPC);
        return ERROR;
    }
    bitsRead = 0;
    for(i=0;i<pInst->numBaseAddr;i++)
    {
        for(j=0;j<2*REL48_REGS;j=j+2)
        {
           bit = 0;
           word = 0;
           do{
                word |= cmd->values[bitsRead++]<<bit;
                bit++;
           }while(REL48_WORD!=bit);
           sysOutWord((ULONG)(pInst->baseAddr[i] + j), word);
        }
    }
    return bitsRead;
}

int trrel48IosIoctl(struct trrel48DrvInst *pInst, int function, int arg)
{
	int i=0;
	if(function == REL48_IO_ADD_BASE_ADR)
	{
		pInst->baseAddr[pInst->numBaseAddr] = (uint32_t*)arg;
		for (i = 0; i < REL48_REGS; i++) 
		{
			pInst->currentState[i] = sysInWord((ULONG)pInst->baseAddr[pInst->numBaseAddr] + 2 * i);
		}		
		pInst->numBaseAddr++;
		return OK;
	}	
    errnoSet(ENOSYS);
    return ERROR;
}
