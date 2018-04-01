#ifndef AR16T16R_REGS_H
#define AR16T16R_REGS_H

/* Порты */
#define XIL_RESET       0  /* Сброс ОЗУ запись */
#define XIL_DAN         4  /* Данные ПЛИС r бит */
#define ADR_RAM         12 /* Адрес БОЗУ r/w слово */
#define DATA_RAM        16 /* Данные БОЗУ r/w слово */
#define MODE            20 /* Режим w байт */
#define OPERATION       24 /* Действие r/w слово */
#define RES_IRQ         28 /* Сброс прерывания w слово */
#define STATUS_IRQ      28 /* Состояние флагов прерывания r слово */
#define EN_IRQ          32 /* Разрешение прерывания w слово */
#define STAT_NUM_BUF    36 /* Состояние активных буферов r слово */

//! типы режимов (MODE), R(only read), W(only write), RW(read and write)
#define REG_START_STOP_TR_RW  	  0x30 /* регистр запуска/останова передатчика*/
#define REG_EN_DIS_RC_RW  		  0x20 /* регистр разрешения/запрещения приемника*/
#define REG_PARAM_RC_W   		  0x40 /* устанавливается параметры приема*/
#define REG_PARAM_TR_W   	 	  0x50 /* устанавливается параметры передачи*/
#define REG_REGZAP_W     		  0x60 /* регистр формирования сигнала "запрос", номер канала 3 бита MODE */
#define REG_TIMER_RW    		  0x80 /* регистр таймера (1-разрешена работа внутреннего таймера), при чтении таймер сбрасывается*/
#define REG_ST_COUNT_TR_R  		  0x00 /* текущее состояние счетчика слов передатчика*/
#define REG_ST_COUNT_RC_R  		  0x10 /* текущее состояние счетчика слов приемника*/
#define REG_COUNT_TR_W    		  0x00 /* сбрасывание счетчика слов, номер буфера передатчика*/
#define REG_COUNT_RC_W    		  0x10 /* сбрасывание счетчика слов, номер буфера приемника*/
#endif
