///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.0.0
	\brief This file contains the functions / constants to work with the peripheral module mdriver

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __ADC_H
#define __ADC_H
#include "stm8l15x_adc.h"

/*!
  \brief This function initializes adc1
*/
void periph_adc1_init(void);
/*!
  \brief This function deinitializes adc1
*/
void periph_adc1_deinit(void);

/*!
  \brief This function gives two bytes by adc1
*/
uint16_t periph_adc1_getvalue(void);


/*!
  \brief This function initializes adc1 with DMA
*/
void periph_adc1_with_DMA_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief This function initializes adc1 with DMA and TIM2
*/
void periph_adc1_with_DMA_and_TIM2_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief This function gives avereage value of one of adc channels
*/
uint8_t periph_adc1_with_DMA_buf_read(uint8_t* buf,uint8_t bufsize, uint8_t channel_column, uint8_t channels_number);

#endif /*__ADC_H */