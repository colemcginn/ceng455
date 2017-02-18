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

#ifdef __cplusplus
extern "C" {
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "message.h"

#define SERIAL_QUEUE 8

typedef struct serial_message {
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char DATA[1];
} SERIAL_MESSAGE, *SERIAL_MESSAGE_PTR;

_pool_id   message_pool;

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
	/* Write your local variable definition here */
	printf("serialTask Created\n\r");

	char buf[13];
	sprintf(buf, "\n\rType here: ");
	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

	SERIAL_MESSAGE_PTR msg_ptr;
	_mqx_uint i;
	_queue_id serial_qid;
	bool result;

	/* open a message queue */
	serial_qid = _msgq_open(SERIAL_QUEUE, 0);

	if (serial_qid == 0) {
		printf("\nCould not open the system message queue\n");
		_task_block();
	}

	/* create a message pool */
	message_pool = _msgpool_create(sizeof(SERIAL_MESSAGE), 7, 0, 0);

	_task_get_error();

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		// Write handler to constantly read message queue and handle messages
//		printf("\n while loop\n");
//		OSA_TimeDelay(10); /* Example code (for task release) */


		// Check queue repeatedly
		msg_ptr = _msgq_receive(serial_qid,0);

//		printf("\n receiving message\n");
		if (msg_ptr == NULL) {
			continue;
			//_task_block();
		}

		printf("message: %c \n", msg_ptr->DATA[0]);
		char c = msg_ptr->DATA[0];

//		msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
//		msg_ptr->HEADER.SOURCE_QID = serial_qid;

//		result = _msgq_send(msg_ptr);
//
//		if (result != TRUE) {
//			printf("\nCould not send a message\n");
//			//_task_block();
//		}
//		sprintf(buf, "\n\rType here: ");
		char buffer[1];
		sprintf(buffer, "%c",c);
		UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
//		UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

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
