Component DRV_RSREL48_VW {
    NAME            RSREL48 VxWorks Driver
    SYNOPSIS        RSREL48 Driver implementation without VxBus   
    INIT_RTN        rsrel48IosInit(); 
    MODULES         rsrel48_ios.o
    PROTOTYPE       void rsrel48IosInit(void);
    INIT_BEFORE     INCLUDE_USER_APPL
    _INIT_ORDER     usrRoot
    _CHILDREN       FOLDER_REL48 
}

