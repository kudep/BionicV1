///\file
/*!
	\author Kuznetsov Denis
	\data 27-08-2016	
	\version 1.1.0
	\brief В this file contains the functions / constants to work with command line

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#include "cmdline.h"
/*!
  \brief this function prints a uint8_t buffer by usart 
*/
void cmd_num_buf_print( char tab[], uint8_t*num_buf, uint8_t num_buf_size)
 {
   char lf='\n';   
   char numb[4];
   for(uint8_t index=0;index<num_buf_size;index++)
   {
     itoa(num_buf[index],numb);
     periph_usart1_bufsend((uint8_t*)numb,strlen(numb));
	 periph_usart1_bufsend(tab,strlen(tab));
   }
   periph_usart1_bufsend((uint8_t*)&lf,1);
 }
/*!
  \brief this function reads some number of arguments by sring(s)
*/
 uint8_t cmd_args_read( char s[], uint8_t*args_buf, uint8_t args_buf_size)
 {
	uint8_t buf_shift=0;
	uint8_t arg_shift=0;
	uint8_t args_buf_num=0;
	bool contin_flag=0;
	for(uint8_t i=0;i<args_buf_size;i++)
	  {
		  contin_flag=TRUE;
		while((!atoi(s+buf_shift,args_buf+args_buf_num,&arg_shift)))
		{
			if(arg_shift!=0)buf_shift+=arg_shift;
			else break;
		}
		if((arg_shift!=0))
                        {
                          args_buf_num++;
                          buf_shift+=arg_shift;
                        }
	  }
	  return args_buf_num;
   
 }
/*!
  \brief  this function converts sring to number 
*/
bool atoi( char s[], uint8_t*n, uint8_t*shift_pos)
 {
     int len;
	 len=strlen(s);
	 (*shift_pos)=0;
	 (*n)=0;
	 if(len==0)return FALSE;
	for(int i=0; i<len;i++)
	{
		if((s[i]>='0')&&(s[i]<='9'))
		{
			(*n)*=10;
			(*n)+=s[i]-'0';
			(*shift_pos)++;
		}
		else
			break;
	}
	if((*shift_pos)==0)
	{
		(*shift_pos)++;
		return FALSE;
	}
	return TRUE;
 }
/*!
  \brief  this function converts number to sring
*/
void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном пор€дке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удал€ем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }
/*!
  \brief  this function maps char positions in string by a reverse
*/
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
	 {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }