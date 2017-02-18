
#include <mqx.h>
#include <message.h>
#include "Cpu.h"
#include "Events.h"
#include "os_tasks.h"
#include "user_task_access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

bool OpenR(_task_id stream_no){
	_queue_id id = _msgq_open(MSGQ_FREE_QUEUE, 0);
	
	//check for prev permission
	for(int i=0;i++;i<num_read){
		if(read_permission[i]->t_id == stream_no){
			//already has read permission
			return false;
		}
	}
	//mutex
	if (_mutex_lock(&openR_mutex) != MQX_OK) {
	 // _task_block();
		// failed mutex
		return false;
	}

	open_permission perm = {stream_no, id};
	read_permission[num_read] = perm;
	num_read++;

	_mutex_unlock(&openR_mutex);
	return true;
}
bool _getline(char *string){
	//Check for permission
	_task_id t_id = _task_get_id();
	_queue_id q_id = 0;
	for(int i=0;i++;i<num_read){
		if(read_permission[i]->t_id == t_id){
			q_id = read_permission[i]->q_id;
		}
	}
	if(q_id == 0){
		//no read permission
		return false;
	}
	//get message
	SERIAL_MESSAGE_PTR msg_ptr;
	msg_ptr = _msgq_receive(serial_qid, 0);
	if (msg_ptr == NULL) {
		//no message
		return false;
	}
	//read line
	strncpy(string, msg_ptr->DATA, size_of_outline);

	_msg_free(msg_ptr);

	return true;

}
_queue_id OpenW(void){
	if(write_permission!=0){
		//permission alreay given for writing
		return 0;
	}

	if (_mutex_lock(&openW_mutex) != MQX_OK) {
		// failed mutex
		return 0;
	}

	write_permission = _task_get_id();

	_mutex_unlock(&openW_mutex);

	return serial_qid;


}
bool _putline(_queue_id qid, char *string){
	//check permission
	if(write_permission != _task_get_id()){
		// no write permission
		return false;
	}
	//append \n
	strcat(string,"\n");

	//make message for entire string
	for(int i=0;i<strlen(string);i++){
		SERIAL_MESSAGE_PTR msg_ptr;

		// Allocate a message
		msg_ptr = (SERIAL_MESSAGE_PTR) _msg_alloc(message_pool);

		if (msg_ptr == NULL) {
			_msg_free(msg_ptr);
			return false;
		}

		msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERIAL_QUEUE); // Set the target Queue ID based on queue number
		msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *) msg_ptr->DATA) + 1; // TODO is this the right size?
		msg_ptr->DATA[0] = string[i];

		result = _msgq_send(msg_ptr);

		_msg_free(msg_ptr);
	}
	return true;


}
bool Close(void){
	bool had_permissions = false
	//check write and remove
	if(write_permission == _task_get_id()){
		write_permission = 0;
		had_permissions = true;
	}

	//check read and remove
	//remake read array
	_task_id t_id = _task_get_id();
	for(int i=0;i++;i<num_read){
		if(read_permission[i]->t_id == t_id){
			num_read--;
			for(int j=i;j<num_read;j++){
				read_permission[j] = read_permission[j+1];
			}
			had_permissions = true;
			break;
		}
	}


	return had_permissions;
}

#ifdef __cplusplus
}  /* extern "C" */
#endif