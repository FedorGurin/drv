#ifndef __REL48_ERRNO_H
#define __REL48_ERRNO_H

typedef enum _REL48_STATUS {
    REL48_SUCCESS,          /* 0: No error - success! */
    REL48_FAIL,             /* 1: Something went wrong (but not defined what) */
    REL48_BAD_PATH,         /* 2: Wrong device file path argument passed */
    REL48_BAD_HANDLE,       /* 3: Bad handle argument passed */
    REL48_BAD_PARAMETER,    /* 4: Generic bad function parameter error */
    REL48_LOW_MEMORY,       /* 5: Memory allocation fail */
    REL48_NOT_INITIALIZED,  /* 6: API not initialized yet (TODO: how to init?) */
    REL48_BAD_BOARD_ID,     /* 7: Wrong board ID (f.e., write attempt in reciever) */
    REL48_BAD_CHANNEL_NO,   /* 8: Channel number too high */
    REL48_NOT_IMPLEMENTED,  /* 9: Not implemented yet */
    REL48_MAXIMUM_ERRNO    /* 10: Maximum error code */
} REL48_STATUS;

#endif /* __REL48_ERRNO_H */ 
