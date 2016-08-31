///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief This file contains the functions / constants to work with the peripheral module usart1

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __SPI_H
#define __SPI_H
#include "stm8l15x_spi.h"

/*!
  \brief This function initializes spi1
*/
void periph_spi1_init(void);
/*!
  \brief This function deinitializes spi1
*/
void periph_spi1_deinit(void);

/*!
  \brief This function transmits buffer by spi1
*/
void periph_spi1_bufsendrecive(uint8_t *buf, uint16_t n);

#endif /*__SPI_H */