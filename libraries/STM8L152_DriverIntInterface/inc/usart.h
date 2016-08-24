///\file
/*!
	\author Kuznetsov Denis
	\data 11-08-2016	
	\version 1.1.0
	\brief � ������ ����� �������� �������/��������� ��� ������ � ������������ ������� usart1

������� ���� ��������� � ���� ����� �������� ����� ���������, ���� �� �������� ��������.
*/
#ifndef __USART_H
#define __USART_H
#include "stm8l15x_usart.h"
#include "stm8l15x_itc.h"
/*!
  \brief ������������� usart1
*/
void periph_usart1_init(void);
/*!
  \brief ��������������� usart1
*/
void periph_usart1_deinit(void);
/*!
  \brief �������� ������� ���� ����� usart1
  \param[in] buf ��������� �� ������������ ������ ����
  \param[in] n ����� ���� ������������ �� ������� ���� ����� uart1
*/
void periph_usart1_bufsend(uint8_t *buf, uint16_t n);
/*!
  \brief ��������� ������� ���� ����� usart1
  \param[in] buf ��������� �� ���������� ������ ����
  \param[in] n ����� ���� ���������� �� ������� ���� ����� uart1
*/
uint8_t periph_usart1_recept(void);

#endif /*__USART_H */