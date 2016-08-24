///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем usart1

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#ifndef __USART_H
#define __USART_H
#include "stm8l15x_usart.h"
#include "stm8l15x_itc.h"
/*!
  \brief инициализация usart1
*/
void periph_usart1_init(void);
/*!
  \brief деинициализация usart1
*/
void periph_usart1_deinit(void);
/*!
  \brief ѕередача массива байт через usart1
  \param[in] buf указатель на передаваемый массив байт
  \param[in] n число байт передаваемое из массива байт через uart1
*/
void periph_usart1_bufsend(uint8_t *buf, uint16_t n);
/*!
  \brief ѕолучение массива байт через usart1
  \param[in] buf указатель на получаемый массив байт
  \param[in] n число байт получаемый из массива байт через uart1
*/
uint8_t periph_usart1_recept(void);

#endif /*__USART_H */