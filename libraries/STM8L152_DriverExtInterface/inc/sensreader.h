///\file
/*!
	\author Kuznetsov Denis
	\data 30-08-2016	
	\version 1.1.0
	\brief  this file contains the functions / constants to work with sensor reader

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __SENSREADER_H
#define __SENSREADER_H

//#define debug

#include "mdriver.h"
#include "adc.h"

typedef enum
{
 cmd_idle     = ((uint8_t)0x00),
 cmd_unclench = ((uint8_t)0x01),
 cmd_clench = ((uint8_t)0x02),
 cmd_conflict = ((uint8_t)0x03),
 cmd_change_mode = ((uint8_t)0x04)  
}ARM_CMD_TypeDef;

typedef struct
{
 uint8_t sensor_level_0;
 uint8_t sensor_level_1;
 uint8_t sensor_level_2;
 uint8_t sensor_level_3;
} Sensor_Levels_TypeDef;

typedef struct
{
 uint8_t skinsens1;
 uint8_t skinsens2;
 uint8_t encsens0;
 uint8_t encsens1;
 uint8_t encsens2;
 uint8_t encsens3;
 uint8_t encsens4;
 uint8_t encsens5;
 uint8_t encsens6;
 uint8_t encsens7;
} Sensor_Channel_TypeDef;

typedef struct
{
	uint8_t mtr_pos0;
	uint8_t mtr_pos1;
	uint8_t mtr_pos2;
	uint8_t mtr_pos3;
	uint8_t mtr_pos4;
	uint8_t mtr_pos5;
	uint8_t mtr_pos6;
	uint8_t mtr_pos7;
} Mtr_Positions_TypeDef;
 
/*!
  \brief this function initializes sensor 1 levels
*/
void sensreader_init_senspin(uint8_t skinsens1,uint8_t skinsens2,
							 uint8_t encsens0,uint8_t encsens1,
							 uint8_t encsens2,uint8_t encsens3,
							 uint8_t encsens4,uint8_t encsens5,
							 uint8_t encsens6,uint8_t encsens7);
/*!
  \brief this function initializes adc
*/
void sensreader_init_adc(uint8_t* _buf, uint8_t _buf_size, uint8_t _ADC_Channels_Number, ADC_Channel_TypeDef _ADC_Channels);

/*!
  \brief this function initializes min/max values
*/
void sensreader_init_min_max(uint32_t delay);

/*!
  \brief this function initializes sensor 1 levels
*/
void sensreader_1levels_init(uint8_t sl0, uint8_t sl1, uint8_t sl2, uint8_t sl3);

/*!
  \brief this function initializes sensor 2 levels
*/
void  sensreader_2levels_init(uint8_t sl0, uint8_t sl1, uint8_t sl2, uint8_t sl3);

/*!
  \brief this function gets command
*/
 void sensreader_get_cmd(ARM_CMD_TypeDef *cmd, uint8_t *value,uint8_t *maxvalue);
/*!
  \brief this function initializes sensor 1 levels
*/
 Mtr_Positions_TypeDef sensreader_get_pos(void);

#endif /*__SENSREADER_H */
