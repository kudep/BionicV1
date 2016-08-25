///\file
/*!
	\author Kuznetsov Denis
	\data 09-08-2016	
	\version 1.0.0
	\brief В данном файле собранны функции/константы для работы с переферийным модулем mdriver

Автором всех сущностей в этом файле является автор документа, если не указанно обратное.
*/
#include "mdriver.h"

static MDriver_TypeDef* MDriverx;
static uint8_t bufsize;



void periph_mdrive_struct_init(MDriver_TypeDef* _MDriverx,GPIO_TypeDef* EN_GPIOx,uint8_t EN_GPIO_Pin,GPIO_TypeDef* PH_GPIOx,uint8_t PH_GPIO_Pin)
{
  _MDriverx->EN_GPIOx=EN_GPIOx;
  _MDriverx->EN_GPIO_Pin=EN_GPIO_Pin;
  _MDriverx->PH_GPIOx=PH_GPIOx;
  _MDriverx->PH_GPIO_Pin=PH_GPIO_Pin;
}

bool periph_mdrive_init(MDriver_TypeDef* _MDriverx,uint8_t _bufsize)
{
  if(_bufsize>Max_Bufsize) return FALSE;
  MDriverx=_MDriverx;
  bufsize=_bufsize;
  for(MDriver_TypeDef* pnrt=MDriverx;pnrt<(MDriverx+bufsize);pnrt++)
  {
    GPIO_Init(pnrt->EN_GPIOx,pnrt->EN_GPIO_Pin,GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(pnrt->PH_GPIOx,pnrt->PH_GPIO_Pin,GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(pnrt->EN_GPIOx,pnrt->EN_GPIO_Pin);
  }
  return TRUE;
}

bool periph_mdrive_task(uint8_t drive_en_reg,uint8_t drive_dir_reg,uint8_t speed)
{
  if(speed>PWM_Period) return FALSE;
  
  MDriver_TypeDef* pnrt;
  uint8_t reg_mask;
  for(int i=0;i<bufsize;i++)
  {
    pnrt=MDriverx+i;
    reg_mask=1<<i;
    if(drive_en_reg&reg_mask)
    {
      if(drive_dir_reg&reg_mask)
      {
        GPIO_SetBits(pnrt->PH_GPIOx,pnrt->PH_GPIO_Pin);
      }
      else
      {
        GPIO_ResetBits(pnrt->PH_GPIOx,pnrt->PH_GPIO_Pin);
      }
      GPIO_SetBits(pnrt->EN_GPIOx,pnrt->EN_GPIO_Pin);
    }
  }
  
  atomTimerDelay(speed);
  
  for(int i=0;i<bufsize;i++)
  {
    pnrt=MDriverx+i;
    GPIO_ResetBits(pnrt->EN_GPIOx,pnrt->EN_GPIO_Pin);
  }
  atomTimerDelay(PWM_Period-speed);
  return TRUE;
}
