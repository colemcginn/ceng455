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
_pool_id task_message_pool;
int num_read;
_queue_id serial_qid;
_queue_id task_qid;
int size_of_outline;
_task_id write_permission;
MUTEX_STRUCT openR_mutex;
MUTEX_STRUCT openW_mutex;
open_permission read_permissions[10];
char output_copy[];
int copy_count;


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
	write_permission = 0;
	_mutex_init(&openR_mutex,NULL);
	_mutex_init(&openW_mutex,NULL);
//	read_permissions


	size_of_outline = 32;
	char buf[size_of_outline];
	output_copy[size_of_outline]; //="" ?
	output_copy[0] = '\0';
	copy_count = 0;
	int copy_count_local = 0;
	char output_copy_local[size_of_outline];// = output_copy;

	sprintf(buf, "\n\rTYPE HERE: ");
	UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

	SERIAL_MESSAGE_PTR msg_ptr;

	SERIAL_MESSAGE_PTR task_msg_ptr;
	bool result;

	// Open a message queue.   _msgq_open(queue_num, max_size);
	serial_qid = _msgq_open(SERIAL_QUEUE, 0);
	printf("serial task qid: %d\n",serial_qid);
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

	task_qid = _msgq_open(9, 0);
	printf("serial task qid: %d\n",task_qid);
	if (task_qid == 0) {
		printf("\nCould not open the system message queue\n");
		_task_block();
	}

	// Create a message pool.   _msgpool_create(msg_size, num_msg, grow_size, grow_lim);
	task_message_pool = _msgpool_create(sizeof(SERIAL_MESSAGE), 7, 0, 0);
	if (task_message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

//	_task_create(0,3,0);

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

//		printf("message: %c \n", msg_ptr->DATA[0]);

		int input_int = msg_ptr->DATA[0];
		char c = msg_ptr->DATA[0];

		// TODO Does this work?
		// TODO How to make it so we can send more messages before it dies?
		char buffer[1];
		sprintf(buffer, "%c",c);
//		printf("%d\n",input_int);
		// ^H=8 ^W=23 ^U=21
		// ^H:erase character, ^W: erase previous word, ^U: erase line
		switch(input_int){
			case 8: // erase character
				printf("erase char\n");
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA[0]), 1000);
				UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
				if(copy_count_local > 0){
					output_copy_local[copy_count_local] = '\0';
					copy_count_local--;
				}
				break;

			case 23: //erase word
				printf("erase word\n");
				while(output_copy_local[copy_count_local] != ' '){
					printf("char: %c\n", output_copy_local[copy_count_local]);
					UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
					UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA[0]), 1000);
					UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
					if(copy_count_local > 0){
						output_copy_local[copy_count_local] = '\0';
						copy_count_local--;
					}
				}
				break;

			case 21: //erase line
				printf("erase line\n");
				while(copy_count_local > 0){
					UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
					UART_DRV_SendDataBlocking(myUART_IDX, " ", sizeof(msg_ptr->DATA[0]), 1000);
					UART_DRV_SendDataBlocking(myUART_IDX, "\b", sizeof(msg_ptr->DATA[0]), 1000);
					output_copy_local[copy_count_local] = '\0';
					copy_count_local--;
				}
				break;
			case 13: //enter is pressed
				/*
				// Allocate a message
				task_msg_ptr = (SERIAL_MESSAGE_PTR) _msg_alloc(task_message_pool);
				if (task_msg_ptr == NULL) {
					_msg_free(msg_ptr);
					return false;
				}
				task_msg_ptr->HEADER.TARGET_QID = task_qid;//qid; // Set the target Queue ID based on queue number
//					msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *) msg_ptr->DATA) + 1; // TODO is this the right size?
				task_msg_ptr->HEADER.SIZE = sizeof(SERIAL_MESSAGE);
				for(int i=0;i<copy_count;i++){
					task_msg_ptr->DATA[i] = output_copy_local[i];
				}
				printf("data: %s\n",task_msg_ptr->DATA);
				printf("target: %d\n",task_msg_ptr->HEADER.TARGET_QID);
				_msgq_send(task_msg_ptr);
//				_msg_free(task_msg_ptr);
				 */

				UART_DRV_SendDataBlocking(myUART_IDX, "\n", sizeof(msg_ptr->DATA[0]), 1000);
				copy_count = copy_count_local;
				while(copy_count_local > 0){
					output_copy[copy_count_local] = output_copy_local[copy_count_local];
					output_copy_local[copy_count_local] = '\0';
					copy_count_local--;
				}
				output_copy[copy_count_local] = output_copy_local[copy_count_local];
				output_copy_local[copy_count_local] = '\0';
				break;

			case 20: //run task
				printf("new task\n");
				_task_create(0,3,0);
			default: //normal input
				UART_DRV_SendDataBlocking(myUART_IDX, buffer, sizeof(buffer), 1000);
				output_copy_local[copy_count_local] = c;
				copy_count_local++;
		}

		_msg_free(msg_ptr);

#ifdef PEX_USE_RTOS
	}
#endif
}


void read_task(os_task_param_t task_init_data){
	printf("read task\n");
	bool result = OpenR(_task_get_id());
	printf("started read\n");
	if(!result){
		printf("failed to get read permission\n");
//		Close();
		_task_block();
	}
	char string1[size_of_outline];
	result = false;
	while(!result) result = _getline(string1);
	printf("Got string %s\n",string1);
	_queue_id resultW = OpenW();
	if(resultW == 0){
		printf("failed to get write permission\n");
//		Close();
		_task_block();
	}
	_putline(resultW,string1);
	bool close_result = Close();
	if(close_result){
		printf("All good, close worked\n");
	} else{
		printf("Error, bad close\n");
	}
	_task_block();
//	// Open a message queue.   _msgq_open(queue_num, max_size);
//		serial_qid = _msgq_open(SERIAL_QUEUE, 0);
//		printf("read task qid: %d\n",serial_qid);
//		if (serial_qid == 0) {
//			printf("\nCould not open the message queue\n");
//			_task_block();
//		}
//
//		// Create a message pool.   _msgpool_create(msg_size, num_msg, grow_size, grow_lim);
//		message_pool = _msgpool_create(sizeof(SERIAL_MESSAGE), 7, 0, 0);
//		if (message_pool == MSGPOOL_NULL_POOL_ID) {
//			printf("\nCould not create a message pool\n");
//			_task_block();
//		}
//
//	bool result = OpenR(_task_get_id());
//	printf("started read\n");
//	if(!result){
//		printf("failed to get read permission\n");
//		return;
//	}
//	char string1[20];
//
//	while(1){
//		result = _getline(string1);
//
//		if(!result){
//			printf("failed to get line\n");
//			break;
//		}
//		printf("Got string %s\n",string1);
//		UART_DRV_SendDataBlocking(myUART_IDX, string1, sizeof(string1), 1000);
//	}
//
//	printf("Got string %s\n",string1);
//
//	UART_DRV_SendDataBlocking(myUART_IDX, string1, sizeof(string1), 1000);
//
//	bool close_result = Close();
//		if(close_result){
//			printf("All good, close worked\n");
//		} else{
//			printf("Error, bad close\n");
//		}
//		return;

}

void write_task(os_task_param_t task_init_data){
//	_queue_id result = OpenW();
//	printf("started write\n");
//	if(result == 0){
//		printf("failed to get write permission\n");
//		return;
//	}
//	char string1[size_of_outline];
//	sprintf(string1, "%s","test");
//	_putline(result,string1);
//	bool close_result = Close();
//	if(close_result){
//		printf("All good, close worked\n");
//	} else{
//		printf("Error, bad close\n");
//	}
//	return;
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
