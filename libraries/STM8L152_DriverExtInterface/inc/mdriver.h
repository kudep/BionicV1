///\file
/*!
	\author Kuznetsov Denis
	\data 9-08-2016	
	\version 1.1.0
	\brief This file contains the functions / constants to work with the peripheral module mdriver

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __MDRIVER_H
#define __MDRIVER_H
#include "stm8l15x_gpio.h"
#include "atomtimer.h"
#define Max_Bufsize     (uint8_t)0x08
#define PWM_Period     (uint8_t)0x05

typedef struct MDriver_struct
{
  GPIO_TypeDef* EN_GPIOx;
  uint8_t EN_GPIO_Pin;
  GPIO_TypeDef* PH_GPIOx;
  uint8_t PH_GPIO_Pin;
} MDriver_TypeDef;

void periph_mdrive_struct_init(MDriver_TypeDef* _MDriverx,GPIO_TypeDef* EN_GPIOx,uint8_t EN_GPIO_Pin,GPIO_TypeDef* PH_GPIOx,uint8_t PH_GPIO_Pin);

bool periph_mdrive_init(MDriver_TypeDef* _MDriverx,uint8_t _bufsize);

bool periph_mdrive_task(uint8_t drive_en_reg,uint8_t drive_dir_reg,uint8_t speed);

#endif /*__MDRIVER_H */
