#ifndef _ADC32_ERRNO_H
#define _ADC32_ERRNO_H

typedef enum {
    ADC32_SUCCESS,                  /* 0 - Success */
    ADC32_FAIL,                     /* 1 - Fail (unknown) */
    ADC32_NO_CONFIG,                /* 2 - Failed to load configuration file */
    ADC32_BAD_CONFIG,               /* 3 - Bad configuration file size */
    ADC32_NOT_INITIALIZED,          /* 4 - Device not initialized yet */
    ADC32_INITIALIZATION_FAILED,    /* 5 - Failed to initialize device */
    ADC32_BAD_PERIOD,               /* 6 - Bad period value */
    ADC32_BAD_CHANNEL,              /* 7 - Bad channel number */
    ADC32_BAD_HANDLE,               /* 8 - Bad handle for working with driver API */
    ADC32_BAD_PARAMETER,            /* 9 - General bad parameter */
    ADC32_BAD_PATH,                 /* 10 - Bad device path */
    ADC32_LOW_MEMORY,               /* 11 - Error with allocation space - probably low mem */
    ADC32_NOT_IMPLEMENTED          /* 12 - Command (ioctl, probably) not implemented */
} ADC32_STATUS;

#endif /* _ADC32_ERRNO_H */
