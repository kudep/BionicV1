///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем usart1

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#ifndef __SPI_H
#define __SPI_H
#include "stm8l15x_spi.h"

/*!
  \brief Инициализация spi1
*/
void periph_spi1_init(void);
/*!
  \brief Деинициализация spi1
*/
void periph_spi1_deinit(void);

/*!
  \brief Передача массива байт через spi1
*/
void periph_spi1_bufsendrecive(uint8_t *buf, uint16_t n);

#endif /*__SPI_H */