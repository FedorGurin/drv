Folder  FOLDER_MONNIK {
    NAME      IKIRAN MONNIK  MIL-1553B
    SYNOPSIS  Monitor IKIRAN   MIL-1553B
    _CHILDREN FOLDER_HARDWARE
}


Component MONNIK_KERNEL_LIB {
    NAME            MONNIK Kernel API Library
    SYNOPSIS        MONNIK API library provided by Nikolaenko    
    MODULES         MonNikApi.o
    INIT_RTN        MonNikLibInit();	
    PROTOTYPE	  STATUS MonNikLibInit(void);
    REQUIRES        INCLUDE_VXBUS \
						INCLUDE_PLB_BUS \
						INCLUDE_PCI_BUS 
    INIT_BEFORE     INCLUDE_VXBUS_DEVICE_CONNECT
    _INIT_ORDER     vxbDevConnect
    _CHILDREN       FOLDER_MONNIK
}

 