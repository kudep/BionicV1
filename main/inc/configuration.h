///\file
/*!
	\author Kuznetsov Denis
	\data 23-08-2016	
	\version 1.0.0
	\brief В данном файле собранны константы, структуры, макросы конфигурации

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H
#include "stm8l15x_adc.h"
/*!
  \brief конфигураци€ adc 
*/
#define ADC_Channels_Number  (uint8_t)0x06

#define Sensor1_ADC_Channel 	ADC_Channel_0	
#define Sensor2_ADC_Channel 	ADC_Channel_1	
#define Encoder1_ADC_Channel	ADC_Channel_2	
#define Encoder2_ADC_Channel	ADC_Channel_3	
#define Encoder3_ADC_Channel	ADC_Channel_4	
#define Encoder4_ADC_Channel	ADC_Channel_5	
#define ADC_Channels			Sensor1_ADC_Channel | Sensor2_ADC_Channel | Encoder1_ADC_Channel | Encoder2_ADC_Channel | Encoder3_ADC_Channel | Encoder4_ADC_Channel

#define ADC_Filter (uint8_t)0x08
#define ADC_Buf_Size (uint8_t)(ADC_Channels_Number*ADC_Filter)
/*!
  \brief индексы устройств дл€ adc буфера 
*/
#define Sensor1 (uint8_t)0x00
#define Sensor2 (uint8_t)0x01
#define Encoder1 (uint8_t)0x02
#define Encoder2 (uint8_t)0x03
#define Encoder3 (uint8_t)0x04
#define Encoder4 (uint8_t)0x05

#endif /*__CONFIGURATION_H */