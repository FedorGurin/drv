#ifndef ARK1620_API_H
#define ARK1620_API_H

#ifdef __cplusplus
extern "C" {
#endif

//! ����� ��� ���������� ARK
STATUS OpenARKAPI(void);

//! ��������� ������� ����� �� ������
STATUS SetOutRkTypesAPI(uint8_t numCh, /*����� ������ �������� ��������� 0..*/
						uint32_t typeRk);	/*��� �� ��� ���������� ������*/

//! �������� ������� ����� �� ������
STATUS SetOutRkAPI(uint8_t numCh,/*����� ������ �������� ��������� 0..*/									
				   uint8_t value);	/*����� ������*/

//! ��������/��������� ������
uint32_t GetInRkAPI(uint8_t numCh) /*������(1)/�������(0)*/

#ifdef __cplusplus
}
#endif

#endif  
