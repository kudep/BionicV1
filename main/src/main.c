/*
 * Copyright (c) 2010, Kelvin Lawson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. No personal names or organizations' names associated with the
 *    Atomthreads project may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ATOMTHREADS PROJECT AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
//#define debug

#include <stdio.h>

#include "atom.h"
#include "atomport-private.h"
#include "atomtimer.h"
#include "atomsem.h"
#include "stm8l15x_conf.h"
#include "usart.h"
#include "adc.h"
#include "mtrcontroller.h"
#include "configuration.h"
#include "cmdline.h"
#include <string.h>


/* Constants */

/*
 * Idle thread stack size
 *
 * This needs to be large enough to handle any interrupt handlers
 * and callbacks called by interrupt handlers (e.g. user-created
 * timer callbacks) as well as the saving of all context when
 * switching away from this thread.
 *
 * In this case, the idle stack is allocated on the BSS via the
 * idle_thread_stack[] byte array.
 */
#define IDLE_STACK_SIZE_BYTES       128


/*
 * Main thread stack size
 *
 * Note that this is not a required OS kernel thread - you will replace
 * this with your own application thread.
 *
 * In this case the Main thread is responsible for calling out to the
 * test routines. Once a test routine has finished, the test status is
 * printed out on the UART and the thread remains running in a loop
 * flashing a LED.
 *
 * The Main thread stack generally needs to be larger than the idle
 * thread stack, as not only does it need to store interrupt handler
 * stack saves and context switch saves, but the application main thread
 * will generally be carrying out more nested function calls and require
 * stack for application code local variables etc.
 *
 * With all OS tests implemented to date on the STM8, the Main thread
 * stack has not exceeded 256 bytes. To allow all tests to run we set
 * a minimum main thread stack size of 204 bytes. This may increase in
 * future as the codebase changes but for the time being is enough to
 * cope with all of the automated tests.
 */
#define MAIN_STACK_SIZE_BYTES       256


/*
 * Startup code stack
 *
 * Some stack space is required at initial startup for running the main()
 * routine. This stack space is only temporarily required at first bootup
 * and is no longer required as soon as the OS is started. By default
 * Cosmic sets this to the top of RAM and it grows down from there.
 *
 * Because we only need this temporarily you may reuse the area once the
 * OS is started, and are free to use some area other than the top of RAM.
 * For convenience we just use the default region here.
 */


/* Local data */

/* Application threads' TCBs */
 static ATOM_TCB main_tcb1;
 static ATOM_TCB main_tcb2;
 static ATOM_TCB main_tcb3;
 static ATOM_TCB main_tcb4;



/* Main thread's stack area (large so place outside of the small page0 area on STM8) */
 __near static uint8_t main_thread_stack1[MAIN_STACK_SIZE_BYTES];
 __near static uint8_t main_thread_stack2[MAIN_STACK_SIZE_BYTES];
 __near static uint8_t main_thread_stack3[MAIN_STACK_SIZE_BYTES];
 __near static uint8_t main_thread_stack4[MAIN_STACK_SIZE_BYTES];

/* Idle thread's stack area (large so place outside of the small page0 area on STM8) */
  __near static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];


/* Forward declarations */
static void main_thread_func1 (uint32_t param);
static void main_thread_func2 (uint32_t param);
static void main_thread_func3 (uint32_t param);
static void main_thread_func4 (uint32_t param);

static ATOM_SEM blinkon_sem;
static ATOM_SEM blinkoff_sem;
void verif(uint8_t status);
 
Mtr_Positions_TypeDef mode0_clench={0x88,0x88,0x00,0x88,0x00,0x88,0x00,0x00};
Mtr_Positions_TypeDef mode0_unclench={0x88,0x88,0x88,0x00,0x88,0xAA,0x00,0x00};
Mtr_Positions_TypeDef mode1_clench={0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x88};
Mtr_Positions_TypeDef mode1_unclench={0x88,0x88,0x88,0x88,0x00,0x00,0x00,0x00};
Mtr_Positions_TypeDef current_pos0={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
Mtr_Positions_TypeDef current_pos1={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

/**
 * \b main
 *
 * Program entry point.
 *
 * Sets up the STM8 hardware resources (system tick timer interrupt) necessary
 * for the OS to be started. Creates an application thread and starts the OS.
 *
 * If the compiler supports it, stack space can be saved by preventing
 * the function from saving registers on entry. This is because we
 * are called directly by the C startup assembler, and know that we will
 * never return from here. The NO_REG_SAVE macro is used to denote such 
 * functions in a compiler-agnostic way, though not all compilers support it.
 *
 */
NO_REG_SAVE void main ( void )
{
    int8_t status;    
    GPIO_Init(GPIOB, GPIO_Pin_0|GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);

    /* Configure GPIO for flashing the STM8S Discovery LED on GPIO D0 */
    

    /**
     * Note: to protect OS structures and data during initialisation,
     * interrupts must remain disabled until the first thread
     * has been restored. They are reenabled at the very end of
     * the first thread restore, at which point it is safe for a
     * reschedule to take place.
     */

    /* Initialise the OS before creating our threads */
    status = atomOSInit(&idle_thread_stack[0], IDLE_STACK_SIZE_BYTES, TRUE);
    if (status == ATOM_OK)
    {
        /* Enable the system tick timer */
        archInitSystemTickTimer();

        /* Create an application thread */
        status = atomThreadCreate(&main_tcb1,
                     16, main_thread_func1, 0,
                     &main_thread_stack1[0],
                     MAIN_STACK_SIZE_BYTES,
                     TRUE);
        status &= atomThreadCreate(&main_tcb2,
                     16, main_thread_func2, 0,
                     &main_thread_stack2[0],
                     MAIN_STACK_SIZE_BYTES,
                     TRUE);
        status &= atomThreadCreate(&main_tcb3,
                     16, main_thread_func3, 0,
                     &main_thread_stack3[0],
                     MAIN_STACK_SIZE_BYTES,
                     TRUE);
        status &= atomThreadCreate(&main_tcb4,
                     16, main_thread_func4, 0,
                     &main_thread_stack4[0],
                     MAIN_STACK_SIZE_BYTES,
                     TRUE);
        if (status == ATOM_OK)
        {
            /**
             * First application thread successfully created. It is
             * now possible to start the OS. Execution will not return
             * from atomOSStart(), which will restore the context of
             * our application thread and start executing it.
             *
             * Note that interrupts are still disabled at this point.
             * They will be enabled as we restore and execute our first
             * thread in archFirstThreadRestore().
             */
            atomOSStart();
            
        }
    }

    /* There was an error starting the OS if we reach here */
    while (1)
    {
    }

}


/**
 * \b main_thread_func
 *
 * Entry point for main application thread.
 *
 * This is the first thread that will be executed when the OS is started.
 *
 * @param[in] param Unused (optional thread entry parameter)
 *
 * @return None
 */
static void main_thread_func1 (uint32_t param)
{
    int sleep_ticks=500;
    GPIO_Init(GPIOE,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(GPIOE,GPIO_Pin_7);
    atomSemCreate (&blinkon_sem, 0);
    atomSemCreate (&blinkoff_sem, 0);
    /* Test finished, flash slowly for pass, fast for fail */
    while (1)
    {
        atomSemGet(&blinkon_sem, 0);
         while(atomSemGet(&blinkoff_sem, -1)==ATOM_WOULDBLOCK)
          {
            GPIO_SetBits(GPIOE,GPIO_Pin_7);
            atomTimerDelay (sleep_ticks);
            GPIO_ResetBits(GPIOE,GPIO_Pin_7);
            atomTimerDelay (sleep_ticks);
          }
    }
}
#define _buf_size      (uint8_t)0x03 
MDriver_TypeDef  MDrivers[_buf_size];
uint8_t drive_en_reg_channel=0;
uint8_t drive_dir_reg_channel=0;
uint8_t speed_channel=0;
static void main_thread_func2 (uint32_t param)
{
  periph_mdrive_struct_init(MDrivers+0,GPIOA,GPIO_Pin_2,GPIOA,GPIO_Pin_3);
  periph_mdrive_struct_init(MDrivers+1,GPIOA,GPIO_Pin_4,GPIOA,GPIO_Pin_5);
  periph_mdrive_struct_init(MDrivers+2,GPIOA,GPIO_Pin_6,GPIOA,GPIO_Pin_7);
  periph_mdrive_init(MDrivers,_buf_size);
    /* Test finished, flash slowly for pass, fast for fail */
    while (1)
    {
        periph_mdrive_task(drive_en_reg_channel,drive_dir_reg_channel,speed_channel);
        
    }
}
static void main_thread_func3 (uint32_t param)
{    
    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Fast);
    while (1)
    {
            GPIO_SetBits(GPIOD,GPIO_Pin_0);
            for(uint8_t i=0;i<40;i++);
            GPIO_ResetBits(GPIOD,GPIO_Pin_0);   
            atomTimerDelay (1);  
    }
}

uint8_t RisC=FALSE;
void test1(void);
void test2(void);
void test3(void);
void test_ini1(void);
static void main_thread_func4 (uint32_t param)
{
    GPIO_Init(GPIOC,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(GPIOE,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);
    GPIO_ResetBits(GPIOE,GPIO_Pin_1);
    //periph_mdrive_init(&MDriver_0);
    //periph_adc1_with_DMA_and_TIM2_init(adc_buf,sizeof(adc_buf),ADC_Channels);
	test_ini1();
    periph_usart1_init();
    while (1)
    {
        if(RisC)
        {
          RisC=FALSE;
		  
          test1();
          test2();
          test3();
        }   
     }
}
 /* char mesg_ok[19]="ATOM_OK \n";
  char mesg_to[19]="ATOM_TIMEOUT\n";
  char mesg_wb[19]="ATOM_WOULDBLOCK \n";
  char mesg_ed[19]="ATOM_ERR_DELETED\n";
  char mesg_ec[19]="ATOM_ERR_CONTEXT \n";
  char mesg_ep[19]="ATOM_ERR_PARAM\n";
  char mesg_eq[19]="ATOM_ERR_QUEUE\n";
  char mesg_et[19]="ATOM_ERR_TIMER \n";

void verif(uint8_t status)
{
        if(status==ATOM_OK )periph_usart1_bufsend((uint8_t*)mesg_ok,strlen(mesg_ok));
        if(status==ATOM_TIMEOUT )periph_usart1_bufsend((uint8_t*)mesg_to,strlen(mesg_to));
        if(status==ATOM_WOULDBLOCK )periph_usart1_bufsend((uint8_t*)mesg_wb,strlen(mesg_wb));
        if(status==ATOM_ERR_DELETED )periph_usart1_bufsend((uint8_t*)mesg_ed,strlen(mesg_ed));
        if(status==ATOM_ERR_CONTEXT )periph_usart1_bufsend((uint8_t*)mesg_ec,strlen(mesg_ec));
        if(status==ATOM_ERR_PARAM )periph_usart1_bufsend((uint8_t*)mesg_ep,strlen(mesg_ep));
        if(status==ATOM_ERR_QUEUE )periph_usart1_bufsend((uint8_t*)mesg_eq,strlen(mesg_eq));
        if(status==ATOM_ERR_TIMER )periph_usart1_bufsend((uint8_t*)mesg_et,strlen(mesg_et));
}
*/
 
#define STANDART_LEN_SRT 5
#define ARG_NUM 5
char buf[STANDART_LEN_SRT]="hi!";
char help[86]="list of commands: \n ledon \n ledoff \n blinkon \n blinkoff \n adcinfo \n adcresult \n";
char about[74]="Atomthreads v1.3 ports on stm8l152 by Kuznetsov Denis \nData: 11-08-2016 \n";
char adcinfo[74]="ADC parametrs:\n      Channels used: 1, 2, 3\n      Sampling rate 400 Hz\n";
#ifdef debug
uint8_t adc_buf[10];
#else
uint8_t adc_buf[ADC_Buf_Size];
#endif
void test1(void)
{
  if(0==strcmp(buf,"ledon"))
              GPIO_SetBits(GPIOC,GPIO_Pin_7);
          if(0==strcmp(buf,"ledoff"))
              GPIO_ResetBits(GPIOC,GPIO_Pin_7);
          
          if(0==strcmp(buf,"blinkoff"))             
              atomSemPut(&blinkoff_sem);
          if(0==strcmp(buf,"blinkon"))              
              atomSemPut(&blinkon_sem);
          
          if(0==strcmp(buf,"help"))
              periph_usart1_bufsend((uint8_t*)help,strlen(help));
          if(0==strcmp(buf,"about"))
              periph_usart1_bufsend((uint8_t*)about,strlen(about));
          if(0==strcmp(buf,"getadcinfo"))
              periph_usart1_bufsend((uint8_t*)adcinfo,strlen(adcinfo));
          if(0==strcmp(buf,"getadcresult"))
          {
            char tab='\t';
            char lf='\n';
            char numb[4];
            periph_usart1_bufsend((uint8_t*)&tab,1);
            periph_usart1_bufsend((uint8_t*)&tab,1);
            for(uint8_t index=0;index<sizeof(adc_buf);index++)
            {
              itoa(adc_buf[index],numb);
              periph_usart1_bufsend((uint8_t*)numb,strlen(numb));
			  periph_usart1_bufsend((uint8_t*)&tab,1);
              periph_usart1_bufsend((uint8_t*)&tab,1);
            }
            periph_usart1_bufsend((uint8_t*)&lf,1);
          }
          if(0==strcmp(buf,"getadcresult1"))
          {
            char lf='\n';
            char numb[8];
            numb[0]=0;
            itoa(periph_adc1_with_DMA_buf_read(adc_buf,sizeof(adc_buf), Encoder2, ADC_Channels_Number),numb);
            periph_usart1_bufsend((uint8_t*)numb,strlen(numb));
            periph_usart1_bufsend((uint8_t*)&lf,1);
          }
}

uint8_t args_buf[ARG_NUM];
uint8_t args_buf_num=0;
void test2(void)
{
          if(0==strncmp(buf,"pushargs",8))
		  {
			periph_usart1_bufsend("ready\n",6);
			args_buf_num=cmd_args_read( buf+8, args_buf, ARG_NUM);
            cmd_num_buf_print( "\t\t", args_buf,args_buf_num);
		  }
          if(0==strncmp(buf,"setstates",9))
          {
			periph_usart1_bufsend("ready\n",6);
			args_buf_num=cmd_args_read( buf+9, args_buf, ARG_NUM);
			uint8_t drive_en_reg;
			uint8_t drive_dir_reg; 
			uint8_t speed;
			if(args_buf[2]==0)	mrt_set_task(&current_pos0,&mode1_clench	,args_buf[0],args_buf[1], &drive_en_reg, &drive_dir_reg, &speed);
			if(args_buf[2]==1)	mrt_set_task(&current_pos0,&mode1_unclench,args_buf[0],args_buf[1], &drive_en_reg, &drive_dir_reg, &speed);
			if(args_buf[2]==2)mrt_set_task(&mode1_unclench,&mode1_clench	,args_buf[0],args_buf[1], &drive_en_reg, &drive_dir_reg, &speed);
			if(args_buf[2]==3)mrt_set_task(&current_pos1,&mode1_unclench,args_buf[0],args_buf[1], &drive_en_reg, &drive_dir_reg, &speed);
			if(args_buf[2]==4)mrt_set_task(&current_pos1,&current_pos1,args_buf[0],args_buf[1], &drive_en_reg, &drive_dir_reg, &speed);
            cmd_num_buf_print( "\t", &drive_en_reg,1);
            cmd_num_buf_print( "\t", &drive_dir_reg,1);
            cmd_num_buf_print( "\t", &speed,1);
          }
}

uint8_t mode_num=0;
#ifdef debug
Mtr_Mode_TypeDef mode_buf[2]={{{2,3,4,5,6,7,8,9},{9,8,7,6,5,4,3,2}},{{12,13,14,15,16,17,18,19},{19,18,17,16,15,14,13,12}}};
#else
Mtr_Mode_TypeDef mode_buf[2]={{{0x88,0x88,0x00,0x88,0x00,0x88,0x00,0x00},{0x88,0x88,0x88,0x00,0x88,0xAA,0x00,0x00}},{{0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x88},{0x88,0x88,0x88,0x88,0x00,0x00,0x00,0x00}}};
#endif

void test3(void)
{
	
          if(0==strcmp(buf,"initsens"))
          {
			periph_usart1_bufsend("ready\n",6);
			sensreader_init_senspin(ADC_Channel_0,ADC_Channel_1,
									ADC_Channel_2,ADC_Channel_3,
									ADC_Channel_0,ADC_Channel_1,
									ADC_Channel_2,ADC_Channel_3,
									ADC_Channel_0,ADC_Channel_1);
#ifdef debug
			sensreader_1levels_init(1, 2, 3, 4);
			sensreader_2levels_init(1, 2, 3, 4);
			sensreader_init_adc(adc_buf, sizeof(adc_buf), 10, ADC_Channels);
#else
			sensreader_init_adc(adc_buf, sizeof(adc_buf), ADC_Channels_Number, ADC_Channels);
#endif
          }
          if(0==strcmp(buf,"catchsenspos"))
          {
			periph_usart1_bufsend("ready\n",6);
          Mtr_Positions_TypeDef temp=sensreader_get_pos();
          cmd_num_buf_print( "\t", &(temp.mtr_pos0),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos1),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos2),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos3),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos4),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos5),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos6),1);
          cmd_num_buf_print( "\t", &(temp.mtr_pos7),1);
          }
          if(0==strcmp(buf,"initmtr"))
          {
			periph_usart1_bufsend("ready\n",6);
			  mtr_init_controller(mode_buf,2);
          }
          if(0==strcmp(buf,"chngmtr"))
          {
			periph_usart1_bufsend("ready\n",6);
			  uint8_t drive_en_reg;
			  uint8_t drive_dir_reg;
			  uint8_t speed;
                          mode_num++;
                          mtr_change_mode(&mode_num);
			  motion_controller(&drive_en_reg, &drive_dir_reg,&speed);
			  cmd_num_buf_print( "\t", &(drive_en_reg),1);
			  cmd_num_buf_print( "\t", &(drive_dir_reg),1);
			  cmd_num_buf_print( "\t", &(speed),1);
			  cmd_num_buf_print( "\t", &(mode_num),1);
          }
          if(0==strcmp(buf,"catchsenscmd"))
          {
			  uint8_t cmd=0;
			  uint8_t value=0;
			  uint8_t maxvalue=0;
			  sensreader_get_cmd(&cmd, &value,&maxvalue);
			  cmd_num_buf_print( "\t", &(cmd),1);
			  cmd_num_buf_print( "\t", &(value),1);
			  cmd_num_buf_print( "\t", &(maxvalue),1);
          }
          if(0==strcmp(buf,"catchmtrcntr"))
          {
			  uint8_t drive_en_reg=0;
			  uint8_t drive_dir_reg=0;
			  uint8_t speed=0;
			  motion_controller(&drive_en_reg, &drive_dir_reg,&speed);
			  cmd_num_buf_print( "\t", &(drive_en_reg),1);
			  cmd_num_buf_print( "\t", &(drive_dir_reg),1);
			  cmd_num_buf_print( "\t", &(speed),1);
          }
		  
#ifdef debug
          if(0==strncmp(buf,"loadadcbuf",10))
          {
			args_buf_num=cmd_args_read( buf+10, adc_buf, 10);
            cmd_num_buf_print( "\t", adc_buf,10);
          }
#endif
}
void test_ini1(void)
{
	sensreader_init_min_max(1);
}
void task_ini(void)
{
	sensreader_init_min_max(10000);
	mtr_init_controller(mode_buf,2);
	sensreader_init_senspin(ADC_Channel_0,ADC_Channel_1,
									ADC_Channel_2,ADC_Channel_3,
									ADC_Channel_0,ADC_Channel_1,
									ADC_Channel_2,ADC_Channel_3,
									ADC_Channel_0,ADC_Channel_1);
	sensreader_1levels_init(1, 2, 3, 4);//????????????
	sensreader_2levels_init(1, 2, 3, 4);//????????????
	sensreader_init_adc(adc_buf, sizeof(adc_buf), ADC_Channels_Number, ADC_Channels);
}
void task(void)
{
	motion_controller(&drive_en_reg_channel, &drive_dir_reg_channel,&speed_channel);
}
