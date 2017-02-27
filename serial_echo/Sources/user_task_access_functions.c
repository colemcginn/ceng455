#include <mqx.h>
#include <message.h>
#include "Cpu.h"
#include "Events.h"
#include "os_tasks.h"
#include "user_task_access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

_queue_id serial_qid;

bool OpenR(_queue_id stream_no){
	//check for prev permission
	for(int i=0;i<num_read;i++){
		if(read_permissions[i].q_id == stream_no){
			//already has read permission
			return false;
		}
	}
	//mutex
	if (_mutex_lock(&openR_mutex) != MQX_OK) {
		// failed mutex
		return false;
	}
	//Add read permission to array
	open_permission perm = {_task_get_id(), stream_no};
	read_permissions[num_read] = perm;
	//Increment count of tasks with read permission
	num_read++;

	_mutex_unlock(&openR_mutex);
	return true;
}
bool _getline(char *string){
	//checks for read permission
	bool result = false;
	for(int i=0;i<num_read;i++){
		if(read_permissions[i].t_id == _task_get_id()){
			//task has read permission
			result = true;
		}
	}
	if(!result){
		// no read permission
		return false;
	}
	//checks to see if there has been a returned line
	if(output_copy[0]=='\0'){
		return false;
	}
	//copies line to string
	for(int i=0;i<copy_count;i++){
		string[i] = output_copy[i];
	}
	return true;
}
_queue_id OpenW(void){
	if(write_permission!=0){
		//permission already given for writing
		return 0;
	}
	if (_mutex_lock(&openW_mutex) != MQX_OK) {
		// failed mutex
		printf("failed mutex write\n");
		return 0;
	}
	//give task write permission
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
	//prints string to terminal
	UART_DRV_SendDataBlocking(myUART_IDX, string, copy_count, 1000);
	return true;
}

bool Close(void){
	bool had_permissions = false;
	//check write and remove
	if(write_permission == _task_get_id()){
		write_permission = 0;
		had_permissions = true;
	}

	//check read and remove
	//remake read array
	_task_id t_id = _task_get_id();
	for(int i=0;i<num_read;i++){
		if(read_permissions[i].t_id == t_id){
			num_read--;
			for(int j=i;j<num_read;j++){
				read_permissions[j] = read_permissions[j+1];
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
