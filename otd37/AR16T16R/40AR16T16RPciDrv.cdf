 
 Component DRV_AR16T16R_PCI {
    NAME            AR16T16R PCI VxBus driver
    SYNOPSIS        PCI AR16T16R VxBus driver
  
					
    MODULES         Ar16t16rPciDriver.o
    INIT_RTN        Ar16t16rPciRegister();
    PROTOTYPE       static void Ar16t16rPciRegister(void);
    REQUIRES        INCLUDE_VXBUS \
						INCLUDE_PLB_BUS \
						INCLUDE_PCI_BUS \
						ARINC_KERNEL_LIB
    CFG_PARAMS      FIRMWARE_CFG_FILE_PATH \
						MAX_FILESIZE_BYTE \
						USE_HEX_FILE

    INIT_AFTER      ARINC_KERNEL_LIB
    _INIT_ORDER     hardWareInterFaceBusInit
    _CHILDREN       FOLDER_ARINC429_16T16R
    
 }
Parameter FIRMWARE_CFG_FILE_PATH {
    NAME        Name of firmware image
    SYNOPSIS    Path to file.
	TYPE        string
	DEFAULT     "/romfs/artmr16.bin"
}
Parameter MAX_FILESIZE_BYTE {
    NAME        Maximum size
    SYNOPSIS    Maximum size of file in bytes
	TYPE        uint
	DEFAULT     200000
}
Parameter USE_HEX_FILE {
    NAME        Use HEX file
    SYNOPSIS    You can use differnt format of firmware
	TYPE        BOOL
	DEFAULT     FALSE
}