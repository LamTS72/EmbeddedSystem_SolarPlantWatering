#include "scheduler.h"

typedef enum
{
	NO_ERROR,
	ERROR_SCH_TOO_MANY_TASKS,
	ERROR_SCH_CANNOT_DELETE_TASK,
	ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK,
	ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER,
	ERROR_SCH_ONE_OR_MORE_SLAVE_DID_NOT_START,
	ERROR_SCH_LOST_SLAVE,
	ERROR_SCH_CAN_BUS_ERROR,
	ERROR_I2C_WRITE_BYTE_AT24C64
}Error_message;

typedef enum
{
	NORMAL,
	ERROR_CODE,
	MAX_TASK
}return_code;

typedef struct {
	void (*pTask)(void);

	uint32_t Delay;

	uint32_t Period;

	uint8_t RunMe;

	uint32_t TaskID;
} sTask;

#define MAX_SIZE 10
#define TASK_ID 0
#define Error_tick_count 0


//initialize array
sTask SCH_tasks_G[MAX_SIZE];
int number_task = 0;

Error_message Errorcode= NO_ERROR;
Error_message Last_error_code = NO_ERROR ;

static unsigned char SCH_Reset_Task(const int TASK_IDX)
{
	unsigned char Return_code;

	if(SCH_tasks_G[TASK_IDX].pTask==0){
		Errorcode= ERROR_SCH_CANNOT_DELETE_TASK;
		return ERROR_CODE;
	}
	else{
		Return_code= NORMAL;
	}

	SCH_tasks_G[TASK_IDX].pTask = 0x0000;
	SCH_tasks_G[TASK_IDX].Delay = 0;
	SCH_tasks_G[TASK_IDX].Period = 0;
	SCH_tasks_G[TASK_IDX].RunMe = 0;

	return Return_code;
}

void SCH_Init(void)
{
	number_task = 0;
	unsigned char i;

	for(i = 0; i < MAX_SIZE ; i++){
		SCH_Reset_Task(i);
		SCH_tasks_G[i].TaskID = i;
	}

	Errorcode = NO_ERROR;

	//Timer_init();
}

void SCH_Update(void) {
	if(number_task == 0) return;
	unsigned char head = 0;
	// NOTE: calculations are in *TICKS* ( not milliseconds )

	if (SCH_tasks_G[head].pTask) {
		if (SCH_tasks_G[head].Delay == 0) {
			SCH_tasks_G[head].RunMe += 1;
		} else {
			SCH_tasks_G[head].Delay -= 1;
		}
	}
}

unsigned char SCH_Add_Task(void (*pFunction)(), unsigned int DELAY, unsigned int PERIOD){
	if (number_task == MAX_SIZE){
		Errorcode = ERROR_SCH_TOO_MANY_TASKS;
		return MAX_TASK;
	}

	unsigned char pivot = 0;
	while (1) {
		if (pivot == number_task) // full
			break;

		if (DELAY < SCH_tasks_G[pivot].Delay)
			break;

		DELAY -= SCH_tasks_G[pivot].Delay;
		pivot++;
	}
	//shift right to have empty slot adding
	for (int i = number_task; i > pivot; i--) {
		SCH_tasks_G[i] = SCH_tasks_G[i - 1];
	}

	//update delay for following pivot position
	if(pivot != number_task)
		SCH_tasks_G[pivot + 1].Delay -= DELAY;

	SCH_tasks_G[pivot].pTask = pFunction;
	SCH_tasks_G[pivot].Delay = DELAY;
	SCH_tasks_G[pivot].Period = PERIOD;
	SCH_tasks_G[pivot].RunMe = 0;
	number_task++;
	return pivot;
}

unsigned char SCH_Delete_Task(unsigned char taskID) {
	unsigned char Return_code ;
	Return_code = SCH_Reset_Task(taskID);
	if(Return_code == NORMAL){
		for (int i = 0; i <number_task - 1; i++) {
				SCH_tasks_G[i] = SCH_tasks_G[i + 1];
			}
		number_task--;
		SCH_tasks_G[number_task].pTask = 0x0000;
		SCH_tasks_G[number_task].Delay = 0;
		SCH_tasks_G[number_task].Period = 0;
		SCH_tasks_G[number_task].RunMe = 0;
	}

	return Return_code;
}

void SCH_Dispatch_Tasks(void) {
	unsigned char head = 0;
	if (SCH_tasks_G[head].RunMe > 0) {
		(*SCH_tasks_G[head].pTask)();
		SCH_tasks_G[head].RunMe -= 1;
		if (SCH_tasks_G[head].Period == 0) {
			SCH_Delete_Task(head);
		} 
        else {
			sTask temp = SCH_tasks_G[head];
			SCH_Delete_Task(head);
			SCH_Add_Task(temp.pTask, temp.Period, temp.Period);
		}
	}

	// SCH_Report_Status();
}



void SCH_Report_Status(void){
#ifdef SCH_REPORT_ERRORS

	if(Errorcode != Last_error_code){
		Error_port = 255 - Errorcode;
		Last_error_code = Errorcode;

		if(Errorcode != 0){
			Error_tick_count = 60000;
		}
		else{
			Error_tick)count = 0;
		}
	}
	else{

		if(Error_tick_count != 0){
			if(--Error_tick_count == 0){
				Errorcode =0;
			}
		}

	}

#endif
}
