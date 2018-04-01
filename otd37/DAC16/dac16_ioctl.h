#ifndef _DAC16_IOCTL_H
#define _DAC16_IOCTL_H


#ifdef __cplusplus
extern "C" {
#endif

#define DAC16_MAGIC 0x7f
    
#define DAC16_IO_ADD_BASE_ADR _IOW(0xeb,  201, int)

#ifdef __cplusplus
}
#endif


#endif /* _DAC16_IOCTL_H */
