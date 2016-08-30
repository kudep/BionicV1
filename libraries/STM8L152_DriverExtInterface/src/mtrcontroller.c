///\file
/*!
	\author Kuznetsov Denis
	\data 30-08-2016	
	\version 1.1.0
	\brief ? this file contains the functions / constants to work with motor conlroller

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#include "mtrcontroller.h"

static Mtr_Mode_TypeDef mode;
static Mtr_Mode_TypeDef* mode_buf;
static uint8_t mode_buf_size;
void mtr_positions_init(Mtr_Positions_TypeDef* mtr_positions,uint8_t mtr_pos0,uint8_t mtr_pos1,uint8_t mtr_pos2,uint8_t mtr_pos3,uint8_t mtr_pos4,uint8_t mtr_pos5,uint8_t mtr_pos6,uint8_t mtr_pos7)
 {
	 mtr_positions->mtr_pos0=mtr_pos0;
	 mtr_positions->mtr_pos1=mtr_pos1;
	 mtr_positions->mtr_pos2=mtr_pos2;
	 mtr_positions->mtr_pos3=mtr_pos3;
	 mtr_positions->mtr_pos4=mtr_pos4;
	 mtr_positions->mtr_pos5=mtr_pos5;
	 mtr_positions->mtr_pos6=mtr_pos6;
	 mtr_positions->mtr_pos7=mtr_pos7;
 }
 
 void mtr_positions_buf_init(Mtr_Positions_TypeDef* mtr_positions,uint8_t *mtr_pos_buf,uint8_t buf_size)
 {
	if(0<buf_size) mtr_positions->mtr_pos0=mtr_pos_buf[0];
	else mtr_positions->mtr_pos0=0;
	if(1<buf_size) mtr_positions->mtr_pos1=mtr_pos_buf[1];
	else mtr_positions->mtr_pos1=0;
	if(2<buf_size) mtr_positions->mtr_pos2=mtr_pos_buf[2];
	else mtr_positions->mtr_pos2=0;
	if(3<buf_size) mtr_positions->mtr_pos3=mtr_pos_buf[3];
	else mtr_positions->mtr_pos3=0;
	if(4<buf_size) mtr_positions->mtr_pos4=mtr_pos_buf[4];
	else mtr_positions->mtr_pos4=0;
	if(5<buf_size) mtr_positions->mtr_pos5=mtr_pos_buf[5];
	else mtr_positions->mtr_pos5=0;
	if(6<buf_size) mtr_positions->mtr_pos6=mtr_pos_buf[6];
	else mtr_positions->mtr_pos6=0;
	if(7<buf_size) mtr_positions->mtr_pos7=mtr_pos_buf[7];
	else mtr_positions->mtr_pos7=0;
 }
 void mtr_init_controller(Mtr_Mode_TypeDef* _mode_buf,uint8_t _mode_buf_size)
 {
	 mode_buf=_mode_buf;
	 mode_buf_size=_mode_buf_size;
	 mtr_change_mode(0);
 }
 void mtr_change_mode(uint8_t* buf_point)
 {
	 (*buf_point)=(*buf_point)%mode_buf_size;
	 mode=mode_buf[(*buf_point)];
 }
 
 void mrt_set_task(Mtr_Positions_TypeDef *from_mtr_positions,Mtr_Positions_TypeDef *to_mtr_positions, uint8_t signal, uint8_t max_signal, uint8_t* drive_en_reg, uint8_t* drive_dir_reg, uint8_t* speed)
 {
	uint8_t temp;
	 if(to_mtr_positions->mtr_pos0<from_mtr_positions->mtr_pos0)temp=from_mtr_positions->mtr_pos0-to_mtr_positions->mtr_pos0,*drive_dir_reg&=~(1<<0);
	 else							   						  temp=to_mtr_positions->mtr_pos0-from_mtr_positions->mtr_pos0, *drive_dir_reg|=(1<<0);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<0);                                                        
	 else  								*drive_en_reg|=(1<<0);                                                    
	 if(to_mtr_positions->mtr_pos1<from_mtr_positions->mtr_pos1)temp=from_mtr_positions->mtr_pos1-to_mtr_positions->mtr_pos1,*drive_dir_reg&=~(1<<1);
	 else							   						  temp=to_mtr_positions->mtr_pos1-from_mtr_positions->mtr_pos1, *drive_dir_reg|=(1<<1);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<1);                                                        
	 else  								*drive_en_reg|=(1<<1);                                                        
	 if(to_mtr_positions->mtr_pos2<from_mtr_positions->mtr_pos2)temp=from_mtr_positions->mtr_pos2-to_mtr_positions->mtr_pos2,*drive_dir_reg&=~(1<<2);
	 else							   						  temp=to_mtr_positions->mtr_pos2-from_mtr_positions->mtr_pos2, *drive_dir_reg|=(1<<2);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<2);                                                        
	 else  								*drive_en_reg|=(1<<2);                                                        
	 if(to_mtr_positions->mtr_pos3<from_mtr_positions->mtr_pos3)temp=from_mtr_positions->mtr_pos3-to_mtr_positions->mtr_pos3,*drive_dir_reg&=~(1<<3);
	 else							   						  temp=to_mtr_positions->mtr_pos3-from_mtr_positions->mtr_pos3, *drive_dir_reg|=(1<<3);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<3);                                                        
	 else  								*drive_en_reg|=(1<<3);                                                        
	 if(to_mtr_positions->mtr_pos4<from_mtr_positions->mtr_pos4)temp=from_mtr_positions->mtr_pos4-to_mtr_positions->mtr_pos4,*drive_dir_reg&=~(1<<4);
	 else							   						  temp=to_mtr_positions->mtr_pos4-from_mtr_positions->mtr_pos4, *drive_dir_reg|=(1<<4);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<4);                                                        
	 else  								*drive_en_reg|=(1<<4);                                                        
	 if(to_mtr_positions->mtr_pos5<from_mtr_positions->mtr_pos5)temp=from_mtr_positions->mtr_pos5-to_mtr_positions->mtr_pos5,*drive_dir_reg&=~(1<<5);
	 else							   						  temp=to_mtr_positions->mtr_pos5-from_mtr_positions->mtr_pos5, *drive_dir_reg|=(1<<5);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<5);                                                        
	 else  								*drive_en_reg|=(1<<5);                                                        
	 if(to_mtr_positions->mtr_pos6<from_mtr_positions->mtr_pos6)temp=from_mtr_positions->mtr_pos6-to_mtr_positions->mtr_pos6,*drive_dir_reg&=~(1<<6);
	 else							   						  temp=to_mtr_positions->mtr_pos6-from_mtr_positions->mtr_pos6, *drive_dir_reg|=(1<<6);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<6);                                                        
	 else  								*drive_en_reg|=(1<<6);                                                        
	 if(to_mtr_positions->mtr_pos7<from_mtr_positions->mtr_pos7)temp=from_mtr_positions->mtr_pos7-to_mtr_positions->mtr_pos7,*drive_dir_reg&=~(1<<7);
	 else							   						  temp=to_mtr_positions->mtr_pos7-from_mtr_positions->mtr_pos7, *drive_dir_reg|=(1<<7);
	 if(temp<MOTOR_POSITION_PRECISION) *drive_en_reg&=~(1<<7);
	 else  								*drive_en_reg|=(1<<7);
	 *speed=(signal*PWM_Period)/max_signal;
	 if(*speed>PWM_Period) *speed=PWM_Period;
 }
 
void motion_controller(uint8_t* drive_en_reg, uint8_t* drive_dir_reg, uint8_t* speed)
{
	static ARM_CMD_TypeDef state;
	static ARM_CMD_TypeDef cmd;
	static uint32_t time_point=0;
	static uint8_t buf_point;
	uint8_t value;
	uint8_t maxvalue;
	Mtr_Positions_TypeDef mtr_pos;
	sensreader_get_cmd(&cmd, &value,&maxvalue);
	switch(cmd)
	{
		case cmd_idle:
		if(IDLE_TIME_DELAY<(atomTimeGet()-time_point)) 
		{	
			//Go to sleep			
		}
		state=cmd_idle;
		break;
		case cmd_clench:
			time_point=atomTimeGet();
			mtr_pos=sensreader_get_pos();
			mrt_set_task(&mtr_pos,&(mode.cleanch_pos),value,maxvalue, drive_en_reg, drive_dir_reg, speed);
			state=cmd_clench;
		//Проверяем позицию каждого пальца и в зависимости от этого определяем двигаться ему или нет, разрешаем вычисление скорости по 1-му сенсору
        case cmd_unclench:
			time_point=atomTimeGet();
			mtr_pos=sensreader_get_pos();
			mrt_set_task(&mtr_pos,&(mode.uncleanch_pos),value,maxvalue, drive_en_reg, drive_dir_reg, speed);
			state=cmd_unclench;
		//Проверяем позицию каждого пальца и в зависимости от этого определяем двигаться ему или нет, разрешаем вычисление скорости по 2-му сенсору
		break;
		case cmd_conflict:
			time_point=atomTimeGet();
			state=cmd_conflict;
		//Останавливаем все ждем разрешения конфликта
		break;
		case cmd_change_mode:
			time_point=atomTimeGet();
			buf_point++;
			mtr_change_mode(&buf_point);
			do
			{
				mtr_pos=sensreader_get_pos();
				mrt_set_task(&mtr_pos,&(mode.uncleanch_pos),maxvalue,maxvalue, drive_en_reg, drive_dir_reg, speed);
			} while((drive_en_reg==0)&&(CHANGE_MODE_TIME_DELAY>(atomTimeGet()-time_point)));
			time_point=atomTimeGet();
			state=cmd_change_mode;
		//Входим в режим изменение положения пальцев согласно следующему режиму, из это го состояния переходим в режим ожидания,пока не сменится режим из состояния никуда не выходим
		break;
		default:
			state=cmd_idle;
		//Входим в режим изменение положения пальцев согласно следующему режиму, из это го состояния переходим в режим ожидания,пока не сменится режим из состояния никуда не выходим
		break;
		
	}
}