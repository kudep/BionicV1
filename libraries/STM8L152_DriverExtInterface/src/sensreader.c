///\file
/*!
	\author Kuznetsov Denis
	\data 30-08-2016	
	\version 1.1.0
	\brief  this file contains the functions / constants to work with sensor reader

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#include "sensreader.h"

static Sensor_Levels_TypeDef sensor1_levels;
static Sensor_Levels_TypeDef sensor2_levels;

static uint8_t* buf;
static uint8_t buf_size;
static uint8_t ADC_Channels_Number;
static Sensor_Channel_TypeDef sensor_channel;

static uint8_t clench_max_value;
static uint8_t unclench_max_value;


void sensreader_init_senspin(uint8_t skinsens1,uint8_t skinsens2,
							 uint8_t encsens0,uint8_t encsens1,
							 uint8_t encsens2,uint8_t encsens3,
							 uint8_t encsens4,uint8_t encsens5,
							 uint8_t encsens6,uint8_t encsens7)
{
	sensor_channel.skinsens1 = skinsens1;
	sensor_channel.skinsens2 = skinsens2;
	sensor_channel.encsens0 = encsens0;
	sensor_channel.encsens1 = encsens1;
	sensor_channel.encsens2 = encsens2;
	sensor_channel.encsens3 = encsens3;
	sensor_channel.encsens4 = encsens4;
	sensor_channel.encsens5 = encsens5;
	sensor_channel.encsens6 = encsens6;
	sensor_channel.encsens7 = encsens7;
}
void sensreader_init_adc(uint8_t* _buf, uint8_t _buf_size, uint8_t _ADC_Channels_Number, ADC_Channel_TypeDef _ADC_Channels)
{
#ifdef debug
	buf=_buf;
	buf_size=_buf_size;
	ADC_Channels_Number=_ADC_Channels_Number;
#else
	buf=_buf;
	buf_size=_buf_size;
	ADC_Channels_Number=_ADC_Channels_Number;
    periph_adc1_with_DMA_and_TIM2_init(buf,buf_size,_ADC_Channels);
#endif
}

void sensreader_init_min_max(uint32_t delay)
{
#ifdef debug
	clench_max_value=20;
	unclench_max_value=10;
#else
	uint8_t _clench_value;
	uint8_t _unclench_value;
	uint32_t timer=atomTimeGet();
	while(delay>(atomTimeGet()-timer))
	{
		_clench_value=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.skinsens1, ADC_Channels_Number);
	    _unclench_value=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.skinsens2, ADC_Channels_Number);
		if(clench_max_value<_clench_value) clench_max_value=_clench_value;
		if(unclench_max_value<_unclench_value) unclench_max_value=_unclench_value;
	}
#endif
	
}
void sensreader_1levels_init(uint8_t sl0, uint8_t sl1, uint8_t sl2, uint8_t sl3)
{
	sensor1_levels.sensor_level_0=sl0;
	sensor1_levels.sensor_level_1=sl1;
	sensor1_levels.sensor_level_2=sl2;
}

void sensreader_2levels_init(uint8_t sl0, uint8_t sl1, uint8_t sl2, uint8_t sl3)
{
	sensor2_levels.sensor_level_0=sl0;
	sensor2_levels.sensor_level_1=sl1;
	sensor2_levels.sensor_level_2=sl2;
	sensor2_levels.sensor_level_3=sl3;
}
 
 void sensreader_get_cmd(ARM_CMD_TypeDef *cmd, uint8_t *value,uint8_t *maxvalue)
{
	
#ifdef debug
	uint8_t sensor1=buf[0];
	uint8_t sensor2=buf[1];
#else
	uint8_t sensor1=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.skinsens1, ADC_Channels_Number);
	uint8_t sensor2=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.skinsens2, ADC_Channels_Number);

#endif
	if((sensor1>sensor1_levels.sensor_level_2)&&(sensor2>sensor2_levels.sensor_level_2))
	{
		(*cmd)=cmd_change_mode;
		return;
	}
	if((sensor1>sensor1_levels.sensor_level_1)&&(sensor2>sensor2_levels.sensor_level_1))
	{
		(*cmd)=cmd_conflict;
		return ;
	}
	if((sensor1>sensor1_levels.sensor_level_1))
	{
		(*cmd)=cmd_clench;
		(*value)=sensor1;
		(*maxvalue)=clench_max_value;
		return;
	}
	if((sensor2>sensor2_levels.sensor_level_1))
	{
		(*cmd)=cmd_unclench;
		(*value)=sensor2;
		(*maxvalue)=unclench_max_value;
		return;
	}
	(*cmd)=cmd_idle;
	return ;
}
 Mtr_Positions_TypeDef sensreader_get_pos(void)
{
	Mtr_Positions_TypeDef temp;
		
#ifdef debug
	if(0<ADC_Channels_Number)  	temp.mtr_pos0=buf[2];																							
	else 						temp.mtr_pos0=0;	 																			
	if(1<ADC_Channels_Number)  	temp.mtr_pos1=buf[3];																							
	else 						temp.mtr_pos1=0;	 																			
	if(2<ADC_Channels_Number)  	temp.mtr_pos2=buf[4];																							
	else 						temp.mtr_pos2=0;	 																			
	if(3<ADC_Channels_Number)  	temp.mtr_pos3=buf[5];																							
	else 						temp.mtr_pos3=0;	 																			
	if(4<ADC_Channels_Number)  	temp.mtr_pos4=buf[6];																							
	else 						temp.mtr_pos4=0;	 																			
	if(5<ADC_Channels_Number)  	temp.mtr_pos5=buf[7];																							
	else 						temp.mtr_pos5=0;	 																			
	if(6<ADC_Channels_Number)  	temp.mtr_pos6=buf[8];																							
	else						temp.mtr_pos6=0;	 																			
	if(7<ADC_Channels_Number)  	temp.mtr_pos7=buf[9];																							
	else						temp.mtr_pos7=0;																							
	return temp;
#else
	if(0<ADC_Channels_Number)  	temp.mtr_pos0=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens0, ADC_Channels_Number);
	else 						temp.mtr_pos0=0;																						
	if(1<ADC_Channels_Number)  	temp.mtr_pos1=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens1, ADC_Channels_Number);
	else 						temp.mtr_pos1=0;																						
	if(2<ADC_Channels_Number)  	temp.mtr_pos2=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens2, ADC_Channels_Number);
	else 						temp.mtr_pos2=0;																						
	if(3<ADC_Channels_Number)  	temp.mtr_pos3=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens3, ADC_Channels_Number);
	else 						temp.mtr_pos3=0;																						
	if(4<ADC_Channels_Number)  	temp.mtr_pos4=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens4, ADC_Channels_Number);
	else 						temp.mtr_pos4=0;																						
	if(5<ADC_Channels_Number)  	temp.mtr_pos5=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens5, ADC_Channels_Number);
	else 						temp.mtr_pos5=0;																						
	if(6<ADC_Channels_Number)  	temp.mtr_pos6=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens6, ADC_Channels_Number);
	else						temp.mtr_pos6=0;																						
	if(7<ADC_Channels_Number)  	temp.mtr_pos7=periph_adc1_with_DMA_buf_read(buf,buf_size, sensor_channel.encsens7, ADC_Channels_Number);
	else						temp.mtr_pos7=0;	
	return temp;
#endif

}																							