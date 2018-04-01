 Folder  FOLDER_DACWAV {
    NAME      DACWAV
    SYNOPSIS  Digital-Analog Converter
    _CHILDREN FOLDER_ADAPTERS
}

Component DRV_DACWAV_VW {
    NAME            DACWAV VxWorks Driver
    SYNOPSIS        DACWAV VxWorks Driver
    MODULES         dacwav.o
    INIT_RTN        dacwavIosInit();
    PROTOTYPE       int dacwavIosInit(void);
    _INIT_ORDER     usrRoot
    INIT_BEFORE     INCLUDE_USER_APPL
    _CHILDREN       FOLDER_DACWAV
    CFG_PARAMS      DACWAV_BASE_ADDRS \
                    DACWAV_DEFAULT_PERIOD \
                    DACWAV_CFG_FILE_PATH \
                    DACWAV_CFG_FILE_SIZE
}