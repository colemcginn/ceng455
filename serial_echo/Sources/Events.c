/* ###################################################################
 **     Filename    : Events.c
 **     Project     : serial_echo
 **     Processor   : MK64FN1M0VLL12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2017-01-30, 15:34, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

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
 **     Callback    : myUART_RxCallback
 **     Description : This callback occurs when data are received.
 **     Parameters  :
 **       instance - The UART instance number.
 **       uartState - A pointer to the UART driver state structure
 **       memory.
 **     Returns : Nothing
 ** ===================================================================
 */
void myUART_RxCallback(uint32_t instance, void * uartState) {
	// Write ISR to add messages to message queue.
	//	UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
	SERIAL_MESSAGE_PTR msg_ptr;
	_queue_id client_qid;
	bool result;

//	client_qid = _msgq_open_system(SERIAL_QUEUE, 0, NULL, 0);
//	client_qid  = _msgq_open((_queue_number)(SERIAL_QUEUE), 0);

	/*allocate a message*/
	msg_ptr = (SERIAL_MESSAGE_PTR) _msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		_msg_free(msg_ptr);
		_task_block();
	}

//	msg_ptr->HEADER.SOURCE_QID = client_qid;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERIAL_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT)
			+ strlen((char *) msg_ptr->DATA) + 1;
	msg_ptr->DATA[0] = myRxBuff[0];

	result = _msgq_send(msg_ptr);

	_msg_free(msg_ptr);

	return;
}

/* END Events */

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
