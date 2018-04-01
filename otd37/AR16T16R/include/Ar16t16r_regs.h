#ifndef AR16T16R_REGS_H
#define AR16T16R_REGS_H

/* ����� */
#define XIL_RESET       0  /* ����� ��� ������ */
#define XIL_DAN         4  /* ������ ���� r ��� */
#define ADR_RAM         12 /* ����� ���� r/w ����� */
#define DATA_RAM        16 /* ������ ���� r/w ����� */
#define MODE            20 /* ����� w ���� */
#define OPERATION       24 /* �������� r/w ����� */
#define RES_IRQ         28 /* ����� ���������� w ����� */
#define STATUS_IRQ      28 /* ��������� ������ ���������� r ����� */
#define EN_IRQ          32 /* ���������� ���������� w ����� */
#define STAT_NUM_BUF    36 /* ��������� �������� ������� r ����� */

//! ���� ������� (MODE), R(only read), W(only write), RW(read and write)
#define REG_START_STOP_TR_RW  	  0x30 /* ������� �������/�������� �����������*/
#define REG_EN_DIS_RC_RW  		  0x20 /* ������� ����������/���������� ���������*/
#define REG_PARAM_RC_W   		  0x40 /* ��������������� ��������� ������*/
#define REG_PARAM_TR_W   	 	  0x50 /* ��������������� ��������� ��������*/
#define REG_REGZAP_W     		  0x60 /* ������� ������������ ������� "������", ����� ������ 3 ���� MODE */
#define REG_TIMER_RW    		  0x80 /* ������� ������� (1-��������� ������ ����������� �������), ��� ������ ������ ������������*/
#define REG_ST_COUNT_TR_R  		  0x00 /* ������� ��������� �������� ���� �����������*/
#define REG_ST_COUNT_RC_R  		  0x10 /* ������� ��������� �������� ���� ���������*/
#define REG_COUNT_TR_W    		  0x00 /* ����������� �������� ����, ����� ������ �����������*/
#define REG_COUNT_RC_W    		  0x10 /* ����������� �������� ����, ����� ������ ���������*/
#endif
