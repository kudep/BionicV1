///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief � ������ ����� �������� �������/��������� ��� ������ � ������������ ������� usart1

������� ���� ��������� � ���� ����� �������� ����� ���������, ���� �� �������� ��������.
*/
#ifndef __SPI_H
#define __SPI_H
#include "stm8l15x_spi.h"

/*!
  \brief ������������� spi1
*/
void periph_spi1_init(void);
/*!
  \brief ��������������� spi1
*/
void periph_spi1_deinit(void);

/*!
  \brief �������� ������� ���� ����� spi1
*/
void periph_spi1_bufsendrecive(uint8_t *buf, uint16_t n);

#endif /*__SPI_H */