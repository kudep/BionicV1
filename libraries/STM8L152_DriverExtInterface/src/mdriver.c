///\file
/*!
	\author Kuznetsov Denis
	\data 09-08-2016	
	\version 1.0.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем usart1

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#include "mdriver.h"

/*!
  \brief Инициализация usart1
*/

void periph_mdrive_init(MDriver_TypeDef* MDriverx)
{
  GPIO_Init(MDriverx->EN_GPIOx,MDriverx->EN_GPIO_Pin,GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(MDriverx->PH_GPIOx,MDriverx->PH_GPIO_Pin,GPIO_Mode_Out_PP_High_Fast);
  GPIO_ResetBits(MDriverx->EN_GPIOx,MDriverx->EN_GPIO_Pin);
}

void periph_mdrive_moveup(MDriver_TypeDef* MDriverx)
{
  GPIO_SetBits(MDriverx->PH_GPIOx,MDriverx->PH_GPIO_Pin);
}

void periph_mdrive_movedown(MDriver_TypeDef* MDriverx)
{
  GPIO_ResetBits(MDriverx->PH_GPIOx,MDriverx->PH_GPIO_Pin);
}

void periph_mdrive_stop(MDriver_TypeDef* MDriverx)
{
  GPIO_ResetBits(MDriverx->EN_GPIOx,MDriverx->EN_GPIO_Pin);
}

void periph_mdrive_start(MDriver_TypeDef* MDriverx)
{
  GPIO_SetBits(MDriverx->EN_GPIOx,MDriverx->EN_GPIO_Pin);
}