/* ###################################################################
 **     Filename    : os_tasks.c
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
 ** @file os_tasks.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup os_tasks_module os_tasks module documentation
 **  @{
 */
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "user_task_access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "message.h"

_pool_id message_pool;

/*
 ** ===================================================================
 **     Callback    : serial_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void serial_task(os_task_param_t task_init_data) {
	printf("serialTask Created\n\r");
	// not sure how to initialize for user tasks
	num_read = 0;
	_queue_id write_permission = 0;

	size_of_outline = 13;
	char buf[size_of_outline];
	char output_copy[size_of_outline]; //="" ?
	int copy_count = 0;

	sprintf(buf, "\n\rType here: ");
	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

	SERIAL_MESSAGE_PTR msg_ptr;
	// _queue_id serial_qid; made global
	bool result;

	// Open a message queue.   _msgq_open(queue_num, max_size);
	serial_qid = _msgq_open(SERIAL_QUEUE, 0);
	if (serial_qid == 0) {
		printf("\nCould not open the system message queue\n");
		_task_block();
	}

	// Create a message pool.   _msgpool_create(msg_size, num_msg, grow_size, grow_lim);
	message_pool = _msgpool_create(sizeof(SERIAL_MESSAGE), 7, 0, 0);
	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		// Handler: Constantly read message queue and handle messages.

		// Check queue
		msg_ptr = _msgq_receive(serial_qid, 0);
		if (msg_ptr == NULL) {
			// continue;
			_task_block();
		}

		printf("message: %c \n", msg_ptr->DATA[0]);
		int input_int = msg_ptr->DATA[0];
		char c = msg_ptr->DATA[0];
		
		// TODO Does this work?
		// TODO How to make it so we can send more messages before it dies?
		char buffer[1];
		sprintf(buffer, "%c",c);
		// ^H=8 ^W=23 ^U=21 
		// ^H:erase character, ^W: erase previous word, ^U: erase line
		switch(input_int){
			case 8: // erase character
				sprintf(buffer, "%c","\b");
				UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
				if(copy_count > 0) copy_count--;

			case 23: //erase word
				while(output_copy[copy_count] != " "){
					sprintf(buffer, "%c","\b");
					UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
					if(copy_count > 0) copy_count--;
				}

			case 21: //erase line
				while(copy_count > 0){
					sprintf(buffer, "%c","\b");
					UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
					copy_count--;
				}
			default: //normal input
				UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
				output_copy[copy_count] = c;
				copy_count++;
		}

		_msg_free(msg_ptr);

#ifdef PEX_USE_RTOS
	}
#endif
}

/* END os_tasks */

#ifdef __cplusplus
} /* extern "C" */
#endif

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
