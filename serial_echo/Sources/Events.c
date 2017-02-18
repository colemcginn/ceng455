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

_pool_id message_pool;

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

 // This is the ISR.
void myUART_RxCallback(uint32_t instance, void * uartState) {
	SERIAL_MESSAGE_PTR msg_ptr;
	bool result;

	// Allocate a message
	msg_ptr = (SERIAL_MESSAGE_PTR) _msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		_msg_free(msg_ptr);
		_task_block();
	}

	// TODO Should we set source?
	// TA said dont bother with source
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERIAL_QUEUE); // Set the target Queue ID based on queue number
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *) msg_ptr->DATA) + 1; // TODO is this the right size?
	msg_ptr->DATA[0] = myRxBuff[0];

	// do we even handle the result?
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
