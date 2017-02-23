/* ###################################################################
**     Filename    : os_tasks.h
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-30, 16:09, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "SerialTask.h"
#include "myUART.h"
#include "ReadTask.h"
#include "WriteTask.h"


#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "user_task_access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data);
void master_read_task(os_task_param_t task_init_data);
void master_write_task(os_task_param_t task_init_data);
typedef void   *_pool_id; // TODO is this supposed to be here?
extern _pool_id message_pool;
extern _pool_id task_message_pool;
extern _queue_id serial_qid;
extern _queue_id task_qid;
extern int size_of_outline;

#define SERIAL_QUEUE 8

typedef struct serial_message {
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char DATA[32];
} SERIAL_MESSAGE, *SERIAL_MESSAGE_PTR;

/*
** ===================================================================
**     Callback    : write_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void write_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : read_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void read_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : read_task2
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void read_task2(os_task_param_t task_init_data);

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif
/* ifndef __os_tasks_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
