///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.0.0
	\brief � ������ ����� �������� �������/��������� ��� ������ � ������������ ������� adc1

������� ���� ��������� � ���� ����� �������� ����� ���������, ���� �� �������� ��������.
*/
#ifndef __ADC_H
#define __ADC_H
#include "stm8l15x_adc.h"

/*!
  \brief ������������� adc1
*/
void periph_adc1_init(void);
/*!
  \brief ��������������� adc1
*/
void periph_adc1_deinit(void);

/*!
  \brief �������� ������� ���� ����� adc1
*/
uint16_t periph_adc1_getvalue(void);


/*!
  \brief ������������� adc1 � DMA
*/
void periph_adc1_with_DMA_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief ������������� adc1 � DMA � TIM2
*/
void periph_adc1_with_DMA_and_TIM2_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels);

/*!
  \brief ������ ������� ������ ������ �� ������ adc1
*/
uint8_t periph_adc1_with_DMA_buf_read(uint8_t* buf,uint8_t bufsize, uint8_t channel_column, uint8_t channels_number);

#endif /*__ADC_H */