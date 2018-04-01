#ifndef AR16T16R_DEFS_H
#define AR16T16R_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct TAR16T16RPCICCONFIG_
{
    uint8_t     bus;     
    uint8_t     func;    
    uint16_t    type;    
    uint32_t    devfn;       
    uint8_t     intNum;  
    uint32_t    baseAddr;   
} TAR16T16RPCICCONFIG;

#define AR16T16R_MAX_DEVICES 8
#define AR16T16R_MAX_CHANNEL_POW_2 4 //16
/*typedef struct MONNIKCONFIG_
{
    UINT8    Unit;

    char     Device[128];
    char     Name[32];
    char     DriverVersion[16];
    UINT8    RevisionId;
    TAR16T16RPCICCONFIG PciConfig;
} MONNIKCONFIG;
*/
#ifdef __cplusplus
}
#endif

#endif
