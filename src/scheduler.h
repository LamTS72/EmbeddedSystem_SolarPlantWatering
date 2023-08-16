#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "stdint.h"
#include "stdio.h"



void SCH_Init(void);

unsigned char SCH_Add_Task(void (*pFunction)(), unsigned int DELAY, unsigned int PERIOD);

unsigned char SCH_Delete_Task(unsigned char taskID);

void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

void SCH_Report_Status(void);


#endif /* INC_SCHEDULER_H_ */