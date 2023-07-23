/*** <semaphore.c> ***/

#include <stdlib.h>
#include <os.h>
#include "semaphore.h"
#include "limits.h"

// ---------------------------------------

sem_t* sem_open (const char *name, int oflag, mode_t mode, unsigned int value)
{
	/* uC/OS-III specific types */
	OS_ERR   err;
	OS_SEM* MySem;
	
	/* Not used param */	
	(void) mode;

	/* Check semaphore limit, semaphore name, and oflag parameter*/	
	if(value > SEM_VALUE_MAX || name == NULL || oflag == 0)
		return (sem_t*)SEM_FAILED;	
	
	/* Allocate memory for semaphore so that it can be share with other tasks" */
	MySem = (OS_SEM*)malloc(sizeof(OS_SEM));

	/* Check if semaphore already exists */
	int sem_exists = 0;//SemaphoreExists(name);

	/* If O_EXCL and O_CREAT are set, sem_open() fails if the semaphore name exists */
	if (oflag == (O_CREAT | O_EXCL) && sem_exists == 1 )
		return (sem_t*)SEM_FAILED;	

	/* Create semaphore because if it does not exists */
	OSSemCreate(MySem, (CPU_CHAR*) name, value, &err);

	if(err == OS_ERR_NONE)
		return(sem_t*)MySem;
	else
		return(sem_t*)SEM_FAILED;
}
