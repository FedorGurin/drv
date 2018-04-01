#ifndef AR16T16R_API_H
#define AR16T16R_API_H

#ifdef __cplusplus
extern "C" {
#endif
//! найти все устройства
STATUS OpenArincAPI(void);

//! общее кол-во каналов
uint16_t numChannelArinc();

//! прочитать двойное слово из канала
STATUS ReadLongWordArincAPI(uint8_t numChannel, /*номер канала сквозна€ нумераци€ 0..*/
						    uint8_t io, 		/*признак типа канала(входной(0)/выходной(1)*/						    
						    uint8_t addr);		/*8-ричный адрес*/

//! записать двойное слово из канала
STATUS WriteLongWordArincAPI(uint8_t numChannel,/*номер канала сквозна€ нумераци€ 0..*/
							 uint8_t io, 		/*признак типа канала(входной(0)/выходной(1)*/							 
						     uint8_t addr,		/*8-ричный адрес*/
						     uint32_t data);	/*слово данных*/

//! задание режима канала передачи
STATUS SetModeTRArincAPI(uint8_t  numChannel,/* номер канала сквозна€ нумераци€ 0..*/
                                uint8_t  numAddr,   /* кол-во адресов передаваемых в канал*/
                                uint8_t *listAddr,  /* список адресов дл€ выдачи каналом*/
                                uint32_t single,    /* выдача 0 - циклическа€, 1 - одинократно*/
                                uint32_t freq,      /* 00 - 50 к√ц, 01 - 100 к√ц, 10 - 12.5 к√ц*/
                                uint32_t interval,  /* пауза между передаваемым словами в тактах частоты(обычно 3)*/
                                uint8_t  switchBuf, /* переключение буферов (1 - разрешает)*/
                                uint8_t  gotov );   /* признак разрешени€/запрещени€ работы по сигналу "«апрос"*/
STATUS SetModeRCArincAPI(uint8_t numChannel,/*номер канала сквозна€ нумераци€ 0..*/
                         uint8_t numAddr,   /* кол-во адресов передаваемых в канал*/
                         uint8_t *listAddr, /* список адресов дл€ выдачи каналом*/                       
                         uint8_t freq,     /* 00 - 50 к√ц, 01 - 100 к√ц, 10 - 12.5 к√ц*/
                         uint8_t switchBuf,/* переключение буфферов (1 - разрешает)*/
                         uint8_t gotov);    /* признак разрешени€/запрещени€ работы по "«апрос" 0- разрешает принимать данные без запроса*/


//! включить/отключить канала
STATUS SetStartArincAPI(uint8_t numChannel,/*номер канала сквозна€ нумераци€ 0..*/
						uint8_t io,		   /*признак типа канала(входной(0)/выходной(1)*/						
						uint8_t value);	   /*запуск(1)/останов(0)*/

//! настройка режима работы канала
STATUS SetModeArincAPI();
#ifdef __cplusplus
}
#endif

#endif  
