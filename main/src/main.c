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


#include <stdio.h>

#include "atom.h"
#include "atomport-private.h"
#include "atomtimer.h"
#include "atomsem.h"
#include "stm8l15x_conf.h"
#include "usart.h"
#include "adc.h"
#include "mdriver.h"
#include "configuration.h"
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
void itoa(int n, char s[]);
void reverse(char s[]);
void PWM_Init(void);
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
static void main_thread_func2 (uint32_t param)
{
  periph_mdrive_struct_init(MDrivers+0,GPIOA,GPIO_Pin_2,GPIOA,GPIO_Pin_3);
  periph_mdrive_struct_init(MDrivers+1,GPIOA,GPIO_Pin_4,GPIOA,GPIO_Pin_5);
  periph_mdrive_struct_init(MDrivers+2,GPIOA,GPIO_Pin_6,GPIOA,GPIO_Pin_7);
  periph_mdrive_init(MDrivers,_buf_size);
    /* Test finished, flash slowly for pass, fast for fail */
    while (1)
    {
        periph_mdrive_task(0x02,0x05,0x2A);
        
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

char buf[99]="hi!";
char help[99]="list of commands: \n ledon \n ledoff \n blinkon \n blinkoff \n adcinfo \n adcresult \n";
char about[99]="Atomthreads v1.3 ports on stm8l152 by Kuznetsov Denis \nData: 11-08-2016 \n";
char adcinfo[99]="ADC parametrs:\n      Channels used: 1, 2, 3\n      Sampling rate 400 Hz\n";
uint8_t RisC=FALSE;
uint8_t adc_buf[ADC_Buf_Size];
//MDriver_TypeDef MDriver_0={GPIOE,GPIO_Pin_1,GPIOE,GPIO_Pin_0};
static void main_thread_func4 (uint32_t param)
{
    GPIO_Init(GPIOC,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(GPIOE,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);
    GPIO_ResetBits(GPIOE,GPIO_Pin_1);
    //periph_mdrive_init(&MDriver_0);
    periph_adc1_with_DMA_and_TIM2_init(adc_buf,sizeof(adc_buf),ADC_Channels);
    periph_usart1_init();
    while (1)
    {
        if(RisC)
        {
          RisC=FALSE;
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
          if(0==strcmp(buf,"adcinfo"))
              periph_usart1_bufsend((uint8_t*)adcinfo,strlen(adcinfo));
         /* if(0==strcmp(buf,"motorup"))
          {
              periph_mdrive_moveup(&MDriver_0);
              periph_mdrive_start(&MDriver_0);
          }
          if(0==strcmp(buf,"motordown"))
          {
              periph_mdrive_movedown(&MDriver_0);
              periph_mdrive_start(&MDriver_0);
          }
          if(0==strcmp(buf,"motorstop"))
              periph_mdrive_stop(&MDriver_0);
          */
          if(0==strcmp(buf,"adcresult"))
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
          if(0==strcmp(buf,"adcresult1"))
          {
            char lf='\n';
            char numb[8];
            numb[0]=0;
            itoa(periph_adc1_with_DMA_buf_read(adc_buf,sizeof(adc_buf), Encoder2, ADC_Channels_Number),numb);
            periph_usart1_bufsend((uint8_t*)numb,strlen(numb));
            periph_usart1_bufsend((uint8_t*)&lf,1);
          }
          
          
        }   
            //atomTimerDelay (1);  
     }
}
  char mesg_ok[19]="ATOM_OK \n";
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
/* itoa:  конвертируем n в символы в s */
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

 /* reverse:  переворачиваем строку s на месте */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
//
//#define Sensor1_Level_0 (uint16_t)0x0032
//#define Sensor1_Level_1 (uint16_t)0x00C8
//#define Sensor1_Level_2 (uint16_t)0x0190
//#define Sensor1_Level_3 (uint16_t)0x02BC
//#define Sensor1_Level_3 (uint16_t)0x02BC
//#define MODE_Number (uint8_t)0x02
//
//
//typedef struct
//{
//  uint16_t Sensor_Level_0;
//  uint16_t Sensor_Level_1;
//  uint16_t Sensor_Level_2;
//  uint16_t Sensor_Level_3;
//} Sensor_Levels_TypeDef;
//uint8_t Mode_Number=MODE_Number;
//Sensor_Levels_TypeDef Sensor_Levels={Sensor1_Level_0,Sensor1_Level_1,Sensor1_Level_2,Sensor1_Level_3};
//
//#define RESET_STATE (uint8_t)0x00
//#define CLENCH_STATE (uint8_t)0x01
//#define UNCLENCH_STATE (uint8_t)0x02
//#define CHANGE_STATE (uint8_t)0x04
//typedef struct
//{
//  uint8_t Mode;
//  uint8_t State;
//  uint8_t Speed;
//} State_TypeDef;
//
//void motion_controller(State_TypeDef &state,uint16_t s1,uint16_t s2)
//{
//  uint16_t senser1=s1;
//  uint16_t senser2=s2;
//  if((senser1>Sensor1_Level_2)&&(senser1>Sensor1_Level_2))
//  {
//    state.Mode=(state.Mode+1)%Mode_Number;
//    state.State=CHANGE_STATE;
//  }
//}


