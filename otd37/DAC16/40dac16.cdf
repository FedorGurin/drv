 Folder  FOLDER_DAC16 {
    NAME      DAC16
    SYNOPSIS  Digital-Analog Converter
    _CHILDREN FOLDER_ADAPTERS
}

Component DRV_DAC16_VW {
    NAME            DAC16 VxWorks Driver
    SYNOPSIS        DAC16 VxWorks Driver
    MODULES         dac16.o
    INIT_RTN        dac16IosInit();
    PROTOTYPE       int dac16IosInit(void);
    _INIT_ORDER     usrRoot
    INIT_BEFORE     INCLUDE_USER_APPL
    _CHILDREN       FOLDER_DAC16
    CFG_PARAMS      DACWAV_BASE_ADDRS \
                    DACWAV_DEFAULT_PERIOD \
                    DACWAV_CFG_FILE_PATH \
                    DACWAV_CFG_FILE_SIZE
}