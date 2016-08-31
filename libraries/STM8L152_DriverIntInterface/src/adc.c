///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.0.0
	\brief This file contains the functions / constants to work with the peripheral module mdriver

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#include "adc.h"

/*!
  \brief This function initializes adc1
*/
void periph_adc1_init(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);///< Џодаем тактирование на adc1
	
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_8Bit, ADC_Prescaler_1); ///< €нициализируем adc1
		
	ADC_ChannelCmd(ADC1, ADC_Channel_2, ENABLE);///< ‚ыбираем канал преобразованиЯ
	
	ADC_Cmd(ADC1, ENABLE); ///< ‚ключаем adc1
}

/*!
  \brief This function deinitializes adc1
*/
void periph_adc1_deinit(void)
{
	ADC_DeInit(ADC1);
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
}

/*!
  \brief This function gives two bytes by adc1
*/
uint16_t periph_adc1_getvalue(void)
{
  ADC_SoftwareStartConv(ADC1);

    while ((ADC1->SR && ADC_SR_EOC) == 0); ///< †дем, пока выполнитсЯ преобразование
	
	return ADC_GetConversionValue(ADC1);
}

/*!
  \brief This function initializes adc1 with DMA
*/
void periph_adc1_with_DMA_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);///< Џодаем тактирование на adc1
	
	ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_8Bit, ADC_Prescaler_1); ///< €нициализируем adc1
		
	ADC_ChannelCmd(ADC1, ADC_Channels, ENABLE);///< ‚ыбираем канал преобразованиЯ
	
	ADC_Cmd(ADC1, ENABLE); ///< ‚ключаем adc1
        
	CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);///< Џодаем тактирование на dma
        DMA_Init(DMA1_Channel0,(uint16_t)buf,(ADC1_BASE+5),bufsize,DMA_DIR_PeripheralToMemory,DMA_Mode_Circular,DMA_MemoryIncMode_Inc,DMA_Priority_Low,DMA_MemoryDataSize_Byte);
        DMA_Cmd(DMA1_Channel0, ENABLE);

        DMA_GlobalCmd(ENABLE);
        ADC_SoftwareStartConv(ADC1);
        
        
// // ----------- Настройка АЦП ---------- 
// 
// ADC1->CR1 |= ADC_CR1_ADON | ADC_CR1_CONT;
//
// ADC1->SQR[3] |= (1<<1) | (1<<2) | (1<<3) | (1<<4); //Выбираем 4 канала
// ADC1->CR2 |= 7; //Время выборки = 384 такта
//
// // ----------- Настройка DMA ----------
// 
// DMA1_Channel0->CNBTR = sizeof(recv_array)-1; //Размер буфера
//  
// DMA1_Channel0->CPARH = (ADC1_BASE+4)>>8; //Адрес регистра АЦП (старший байт)
// DMA1_Channel0->CPARL = (uint8_t)(ADC1_BASE+4); //Младший
// 
// DMA1_Channel0->CM0ARH = (uint8_t)((uint16_t)recv_array>>8); //Адрес буфера
// DMA1_Channel0->CM0ARL = (uint8_t)recv_array;  
//  
// DMA1_Channel0->CSPR |= DMA_CSPR_16BM;  //Режим работы с 16и битными числами.
// DMA1_Channel0->CCR |= DMA_CCR_IDM | DMA_CCR_CE | DMA_CCR_TCIE; //Включаем канал, одно преобразование и разрешаем прерывание
// DMA1->GCSR |= DMA_GCSR_GE; //Включаем DMA
//
// enableInterrupts(); //Разрешаем прерывания
// 
// ADC1->CR1 |= ADC_CR1_START; //Начинаем измерение 
}

/*!
  \brief This function initializes adc1 with DMA and TIM2
*/
void periph_adc1_with_DMA_and_TIM2_init(uint8_t* buf,uint8_t bufsize, ADC_Channel_TypeDef ADC_Channels)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);///< Џодаем тактирование на adc1
	
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_8Bit, ADC_Prescaler_1); ///< €нициализируем adc1
		
	ADC_ChannelCmd(ADC1,ADC_Channels, ENABLE);///< ‚ыбираем канал преобразованиЯ
        ADC_ExternalTrigConfig(ADC1,ADC_ExtEventSelection_Trigger3,ADC_ExtTRGSensitivity_Rising);
	
	ADC_Cmd(ADC1, ENABLE); ///< ‚ключаем adc1
        
	CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);///< Џодаем тактирование на dma
        DMA_Init(DMA1_Channel0,(uint16_t)buf,(ADC1_BASE+5),bufsize,DMA_DIR_PeripheralToMemory,DMA_Mode_Circular,DMA_MemoryIncMode_Inc,DMA_Priority_Low,DMA_MemoryDataSize_Byte);
        //DMA_ITConfig(DMA1_Channel0, DMA_ITx_TC, ENABLE);//Отключено прерывание по заввершения прерыванию
        DMA_Cmd(DMA1_Channel0, ENABLE);

        DMA_GlobalCmd(ENABLE);
       // ADC_SoftwareStartConv(ADC1);

	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);///< Џодаем тактирование на TIM2
        TIM2_TimeBaseInit(TIM2_Prescaler_128,TIM2_OCMode_Timing,314);
        TIM2_UpdateRequestConfig(TIM2_UpdateSource_Regular);
        TIM2_SelectOutputTrigger(TIM2_TRGOSource_Update);

        TIM2_Cmd(ENABLE);
 //ADC1->CR2 |= (1<<5) | (3<<3); //Старт по переднему фронту от таймера.
 //ADC1->CR3 |= (4<<5); //Sampling time = 12uS
//  //Волшебное число 7812 получилось так:
//  // Частота МК (2М) / Предделитель таймера (128) * Нужное время в секундах (0.5)
  //TIM2->ARRH = 314>>8; //Записываем сначала старший байт
  //TIM2->ARRL = (uint8_t)(314); //Потом младший
 // TIM2->CR1 |= TIM_CR1_URS; //Настраиваем источник события update
  //TIM2->CR2 |= (2<<4); //Настраиваем на подачу сигнала для АЦП при переполнении таймера
  //TIM2->PSCR = 7; //Предделитель = 2^7 = 128
  //TIM2->EGR |= TIM_EGR_UG;  //Геннерим событие update, чтобы обновился предделитель
 // TIM2->CR1 |= TIM_CR1_CEN; //Запускаем
        
}

/*!
  \brief This function gives avereage value of one of adc channels
*/
uint8_t periph_adc1_with_DMA_buf_read(uint8_t* buf,uint8_t bufsize, uint8_t channel_column, uint8_t channels_number)
{
  uint16_t temp=0;
  for(uint8_t i=channel_column;i<bufsize;i+=channels_number)
  {
    temp+=buf[i];
  }
  return (uint8_t)(temp>>3);
} 