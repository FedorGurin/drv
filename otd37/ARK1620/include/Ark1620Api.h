#ifndef ARK1620_API_H
#define ARK1620_API_H

#ifdef __cplusplus
extern "C" {
#endif

//! найти все устройства ARK
STATUS OpenARKAPI(void);

//! прочитать двойное слово из канала
STATUS SetOutRkTypesAPI(uint8_t numCh, /*номер канала сквозная нумерация 0..*/
						uint32_t typeRk);	/*тип РК для указанного канала*/

//! записать двойное слово из канала
STATUS SetOutRkAPI(uint8_t numCh,/*номер канала сквозная нумерация 0..*/									
				   uint8_t value);	/*слово данных*/

//! включить/отключить канала
uint32_t GetInRkAPI(uint8_t numCh) /*запуск(1)/останов(0)*/

#ifdef __cplusplus
}
#endif

#endif  
