///\file
/*!
	\author Kuznetsov Denis
	\data 09-08-2016	
	\version 1.0.0
	\brief � ������ ����� �������� �������/��������� ��� ������ � ������������ ������� spi1

������� ���� ��������� � ���� ����� �������� ����� ���������, ���� �� �������� ��������.
*/
#include "spi.h"

/*!
  \brief ������������� spi1
*/
void periph_spi1_init(void)
{
  GPIO_Init (GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT); 
  GPIO_Init (GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast); 
  GPIO_Init (GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast); 

  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
 
  SPI_DeInit(SPI1);
  SPI_Init(SPI1,SPI_FirstBit_MSB,SPI_BaudRatePrescaler_256,SPI_Mode_Master,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft,0x00);
  SPI_Cmd(SPI1,ENABLE); 
}

/*!
  \brief ��������������� spi1
*/
void periph_spi1_deinit(void)
{
  GPIO_Init (GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT); 
  GPIO_Init (GPIOB, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT); 
  GPIO_Init (GPIOB, GPIO_Pin_5, GPIO_Mode_In_FL_No_IT); 
  SPI_DeInit(SPI1);
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);
}

/*!
  \brief �������� ������� ���� ����� spi1
*/
void periph_spi1_bufsendrecive(uint8_t *buf, uint16_t n)
{
  for(uint16_t i=0; i<n; i++)
  {
    while ((SPI1->SR & (u8)SPI_FLAG_TXE) == RESET) { ; }///< ������� ������������ ��������� �������� TDR
    SPI1->DR=buf[i]; ///< ���������� i-� ���� �� �������
    while ((SPI1->SR & (u8)SPI_FLAG_RXNE) == RESET) { ; }
    buf[i]=SPI_ReceiveData(SPI1);
  }
}