Folder  FOLDER_ADC32 {
    NAME        ADC32 Board
    SYNOPSIS    ADC32 Board
    _CHILDREN   FOLDER_ADAPTERS
}

Component DRV_ADC32_VW {
    NAME            ADC32 VxWorks Driver 
    SYNOPSIS        ADC32 VxWorks Driver (IO system, without VxBus).\
					The board is on the ISA bus. To set addresses of the boards, \
					modify `base addresses` 
    SOURCE          ADC32
    MODULES         adc32Ios.o
    INIT_RTN        iosAdc32Init();
    PROTOTYPE       int iosAdc32Init(void);
    INIT_BEFORE     INCLUDE_USER_APPL
    _INIT_ORDER     usrRoot
    CFG_PARAMS      ADC32_BASE_ADDRS \
                    ADC32_ORDERING
    _CHILDREN       FOLDER_ADC32        
}

