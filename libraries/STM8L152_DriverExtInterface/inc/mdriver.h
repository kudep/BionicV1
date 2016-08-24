///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем mdrive

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#ifndef __MDRIVER_H
#define __MDRIVER_H
#include "stm8l15x_gpio.h"

typedef struct MDriver_struct
{
  GPIO_TypeDef* EN_GPIOx;
  uint8_t EN_GPIO_Pin;
  GPIO_TypeDef* PH_GPIOx;
  uint8_t PH_GPIO_Pin;
} MDriver_TypeDef;


void periph_mdrive_init(MDriver_TypeDef* MDriverx);

void periph_mdrive_moveup(MDriver_TypeDef* MDriverx);

void periph_mdrive_movedown(MDriver_TypeDef* MDriverx);

void periph_mdrive_stop(MDriver_TypeDef* MDriverx);
void periph_mdrive_start(MDriver_TypeDef* MDriverx);

#endif /*__MDRIVER_H */
