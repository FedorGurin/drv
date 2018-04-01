Component DRV_MONNIK_PCI {
    NAME            MONNIK PCI VxBus driver
    SYNOPSIS        PCI MONNIK VxBus driver provided by NIKOLAENKO
    MODULES         MonNikPciDriver.o					
    INIT_RTN        MonNikPciRegister();
    PROTOTYPE       void MonNikPciRegister(void);
    REQUIRES        INCLUDE_VXBUS \
						INCLUDE_PLB_BUS \
						INCLUDE_PCI_BUS \
						MONNIK_KERNEL_LIB
    INIT_AFTER      MONNIK_KERNEL_LIB
    _INIT_ORDER     hardWareInterFaceBusInit
    _CHILDREN       FOLDER_MONNIK
    
 }