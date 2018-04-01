#ifndef AR16T16R_API_H
#define AR16T16R_API_H

#ifdef __cplusplus
extern "C" {
#endif
//! ����� ��� ����������
STATUS OpenArincAPI(void);

//! ����� ���-�� �������
uint16_t numChannelArinc();

//! ��������� ������� ����� �� ������
STATUS ReadLongWordArincAPI(uint8_t numChannel, /*����� ������ �������� ��������� 0..*/
						    uint8_t io, 		/*������� ���� ������(�������(0)/��������(1)*/						    
						    uint8_t addr);		/*8-������ �����*/

//! �������� ������� ����� �� ������
STATUS WriteLongWordArincAPI(uint8_t numChannel,/*����� ������ �������� ��������� 0..*/
							 uint8_t io, 		/*������� ���� ������(�������(0)/��������(1)*/							 
						     uint8_t addr,		/*8-������ �����*/
						     uint32_t data);	/*����� ������*/

//! ������� ������ ������ ��������
STATUS SetModeTRArincAPI(uint8_t  numChannel,/* ����� ������ �������� ��������� 0..*/
                                uint8_t  numAddr,   /* ���-�� ������� ������������ � �����*/
                                uint8_t *listAddr,  /* ������ ������� ��� ������ �������*/
                                uint32_t single,    /* ������ 0 - �����������, 1 - �����������*/
                                uint32_t freq,      /* 00 - 50 ���, 01 - 100 ���, 10 - 12.5 ���*/
                                uint32_t interval,  /* ����� ����� ������������ ������� � ������ �������(������ 3)*/
                                uint8_t  switchBuf, /* ������������ ������� (1 - ���������)*/
                                uint8_t  gotov );   /* ������� ����������/���������� ������ �� ������� "������"*/
STATUS SetModeRCArincAPI(uint8_t numChannel,/*����� ������ �������� ��������� 0..*/
                         uint8_t numAddr,   /* ���-�� ������� ������������ � �����*/
                         uint8_t *listAddr, /* ������ ������� ��� ������ �������*/                       
                         uint8_t freq,     /* 00 - 50 ���, 01 - 100 ���, 10 - 12.5 ���*/
                         uint8_t switchBuf,/* ������������ �������� (1 - ���������)*/
                         uint8_t gotov);    /* ������� ����������/���������� ������ �� "������" 0- ��������� ��������� ������ ��� �������*/


//! ��������/��������� ������
STATUS SetStartArincAPI(uint8_t numChannel,/*����� ������ �������� ��������� 0..*/
						uint8_t io,		   /*������� ���� ������(�������(0)/��������(1)*/						
						uint8_t value);	   /*������(1)/�������(0)*/

//! ��������� ������ ������ ������
STATUS SetModeArincAPI();
#ifdef __cplusplus
}
#endif

#endif  
