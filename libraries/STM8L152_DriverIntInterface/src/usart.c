///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief This file contains the functions / constants to work with the peripheral module usart1

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#include "usart.h"

/*!
  \brief This function initializes usart1
*/
void periph_usart1_init(void)
{
  
  /* Enable USART clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  
  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);
  
  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);
  
  USART_Init(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
                   USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  ITC_SetSoftwarePriority(USART1_RX_IRQn,ITC_PriorityLevel_3);
}

/*!
  \brief This function deinitializes usart1
*/
void periph_usart1_deinit(void)
{
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, DISABLE);
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, DISABLE);
  USART_DeInit(USART1);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
}

/*!
  \brief This function transmits buffer by usart1
*/
void periph_usart1_bufsend(uint8_t *buf, uint16_t n)
{
  for(uint16_t i=0; i<n; i++)
  {
   while(!(USART1->SR&USART_SR_TXE)); ///< Ожидаем освобождения буферного регистра TDR
   USART1->DR=buf[i]; ///< Отправляем i-й байт из массива
  }
}
/*!
  \brief This function receives one byte by usart1
*/
uint8_t periph_usart1_recept(void)
{
    while ((USART1->SR & (u8)USART_SR_RXNE) == RESET) { ; }
    return USART1->DR;
}