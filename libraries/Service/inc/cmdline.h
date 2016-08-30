///\file
/*!
	\author Kuznetsov Denis
	\data 27-08-2016	
	\version 1.1.0
	\brief ‚ this file contains the functions / constants to work with command line

The author of all the entities in this file is the author of the document, unless indicated otherwise.
*/
#ifndef __CMDLINE_H
#define __CMDLINE_H
#include "usart.h"
/*!
  \brief this function prints a uint8_t buffer by usart 
*/
void cmd_num_buf_print( char tab[], uint8_t*num_buf, uint8_t num_buf_size);
/*!
  \brief this function reads some number of arguments by sring(s)
*/
uint8_t cmd_args_read( char s[], uint8_t*args_buf, uint8_t args_buf_size);
/*!
  \brief  this function converts sring to number 
*/
bool atoi( char s[], uint8_t*n, uint8_t*shift_pos);
/*!
  \brief  this function converts number to sring
*/
void itoa(int n, char s[]);
/*!
  \brief  this function maps char positions in string by a reverse
*/
void reverse(char s[]);

#endif /*__CMDLINE_H */