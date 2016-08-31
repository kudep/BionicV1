///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief This file contains the functions / constants to work with the peripheral module usart1

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __USART_H
#define __USART_H
#include "stm8l15x_usart.h"
#include "stm8l15x_itc.h"
/*!
  \brief This function initializes usart1
*/
void periph_usart1_init(void);
/*!
  \brief This function deinitializes usart1
*/
void periph_usart1_deinit(void);
/*!
  \brief This function transmits buffer by usart1
*/
void periph_usart1_bufsend(uint8_t *buf, uint16_t n);
/*!
  \brief This function receives one byte by usart1
*/
uint8_t periph_usart1_recept(void);

#endif /*__USART_H */