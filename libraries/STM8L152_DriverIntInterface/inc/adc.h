///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.0.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем adc1

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#ifndef __ADC_H
#define __ADC_H
#include "stm8l15x_adc.h"

/*!
  \brief инициализация adc1
*/
void periph_adc1_init(void);
/*!
  \brief деинициализация adc1
*/
void periph_adc1_deinit(void);

/*!
  \brief Передача массива байт через adc1
*/
uint16_t periph_adc1_getvalue(void);


/*!
  \brief Инициализация adc1 с DMA
*/
void periph_adc1_with_DMA_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief Инициализация adc1 с DMA и TIM2
*/
void periph_adc1_with_DMA_and_TIM2_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief „тение столбца одного канала из буфера adc1
*/
uint8_t periph_adc1_with_DMA_buf_read(uint8_t* buf,uint8_t bufsize, uint8_t channel_column, uint8_t channels_number);

#endif /*__ADC_H */