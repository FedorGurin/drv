Component DRV_TRREL48_VW {
    NAME            TRREL48 VxWorks Driver
    SYNOPSIS        TRREL48 Driver implementation without VxBus  
    INIT_RTN        trrel48IosInit(); 
    MODULES         trrel48_ios.o
    PROTOTYPE       void trrel48IosInit(void);
    INIT_BEFORE     INCLUDE_USER_APPL
    _INIT_ORDER     usrRoot
    _CHILDREN       FOLDER_REL48   
}