#include <errnoLib.h>
#include <errnoLib.h>

#include <stdio.h>
#include <stdlib.h>

#include "adc32Defs.h"

#define ADC32_CFG_PATH  "/romfs/cfg/adc32.bit"
#define ADC32_CFG_SIZE  42167

int adc32GetConfig(char **buffer)
{
    static char *me = "adc32GetConfig()";
    static char *config = NULL;
    static int configSize = 0;
    static int errorReported = 0;
    
    FILE *f;
    int bytesRead = 0;
    
    if (config == NULL) {
        config = (char *) malloc(sizeof(char) * ADC32_CFG_SIZE);
        if (config == NULL) {
            return ERROR;
        }
        f = fopen(ADC32_CFG_PATH, "rb");
        if (f == NULL) {
            free(config);
            config = NULL;
            return ERROR;
        }
        while (configSize != ADC32_CFG_SIZE) {
            bytesRead = fread(config + configSize, sizeof(char), ADC32_CFG_SIZE, f);
            if (!bytesRead) {
                /* Determine, what error occured, only if it wasn't reported yet */
                if (!errorReported) {
                    if (feof(f))
                        logMsg("%s: encountered end of error while loading configuration. Total bytes read: %d, expected %d.", 
                                (int)me, (int)configSize, (int)ADC32_CFG_SIZE, 0, 0, 0);
                    else if (ferror(f))
                        logMsg("%s: some error occured while loading confugration. Please, check that file exists and accesible at %s",
                                (int)me, (int)ADC32_CFG_PATH, 0, 0, 0, 0);
                    errorReported = !errorReported;
                }
                free(config);
                config = NULL;
                configSize = 0;
                fclose(f);
            }
            configSize += bytesRead;
        }
        fclose(f);
    }
    *buffer = config;
    return configSize;
}
