///\file
/*!
	\author Kuznetsov Denis
	\data 30-08-2016	
	\version 1.1.0
	\brief  This file contains the functions / constants to work with motor conlroller

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __MTRCONTROLLER_H
#define __MTRCONTROLLER_H
#include "mdriver.h"
#include "sensreader.h"


//#define debug
#ifdef debug
#define MOTOR_POSITION_PRECISION	(int8_t)0x1
#else
#define MOTOR_POSITION_PRECISION	(int8_t)0x10
#endif	
#define MOTOR_NUMBER	(int8_t)0x05
#define IDLE_TIME_DELAY	(int32_t)0xEA60
#define CHANGE_MODE_TIME_DELAY	(int32_t)0x2710

 typedef struct
 {
	 Mtr_Positions_TypeDef  cleanch_pos;
	 Mtr_Positions_TypeDef  uncleanch_pos;
 }Mtr_Mode_TypeDef;
/*!
  \brief This function initializes a motor position by 8 numbers
*/
void mtr_positions_init(Mtr_Positions_TypeDef* mtr_positions,uint8_t mtr_pos0,uint8_t mtr_pos1,uint8_t mtr_pos2,uint8_t mtr_pos3,uint8_t mtr_pos4,uint8_t mtr_pos5,uint8_t mtr_pos6,uint8_t mtr_pos7);
 
  
/*!
  \brief This function initializes a motor position by buffer
*/
void mtr_positions_buf_init(Mtr_Positions_TypeDef* mtr_positions,uint8_t *mtr_pos_buf,uint8_t buf_size);


/*!
  \brief This function initializes motor controller 
*/
void mtr_init_controller(Mtr_Mode_TypeDef* _mode_buf,uint8_t _mode_buf_size);
  

/*!
  \brief This function changes work mode by mode of buffer with number buf_point 
*/
 void mtr_change_mode(uint8_t* buf_point);
  

/*!
  \brief This function sets state registers
*/
void mrt_set_task(Mtr_Positions_TypeDef* from_mtr_positions,Mtr_Positions_TypeDef* to_mtr_positions, uint8_t signal, uint8_t max_signal, uint8_t* drive_en_reg, uint8_t* drive_dir_reg, uint8_t* speed);

/*!
  \brief This function handles a command
*/
void motion_controller(uint8_t* drive_en_reg, uint8_t* drive_dir_reg, uint8_t* speed);

#endif /*__MTRCONTROLLER_H */
