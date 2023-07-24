/*** <semaphore.c> ***/

#include <stdlib.h>
#include <os.h>
#include "semaphore.h"
#include "limits.h"

// ---------------------------------------

sem_t* msc_sem_open (const char *name, int oflag, mode_t mode, unsigned int value)
{
	/* uC/OS-III specific types */
	OS_ERR   err;
	OS_SEM* MySem;
	
	/* Not used param */	
	(void) mode;

	/* Check semaphore limit, semaphore name, and oflag parameter*/	
	if(value > SEM_VALUE_MAX || name == NULL || oflag == 0)
		return (sem_t*)SEM_OPEN_FAIL;
	
	/* Allocate memory for semaphore so that it can be share with other tasks" */
	MySem = (OS_SEM*)malloc(sizeof(OS_SEM));

	/* Check if semaphore already exists */
	int sem_exists = 0;//SemaphoreExists(name);

	/* If O_EXCL and O_CREAT are set, sem_open() fails if the semaphore name exists */
	if (oflag == (O_CREAT | O_EXCL) && sem_exists == 1 )
		return (sem_t*)SEM_OPEN_FAIL;

	/* Create semaphore because if it does not exists */
	OSSemCreate(MySem, (CPU_CHAR*) name, value, &err);

	if(err == OS_ERR_NONE)
		return(sem_t*)MySem;
	else
		return(sem_t*)SEM_OPEN_FAIL;
}


int msc_sem_wait(sem_t *sem)
{
	/* uC/OS-III specific types */
	OS_ERR   err;

	if(sem == 0)
		return SEM_WAIT_FAIL;

	/* Posix to uC/OS-III semaphore compatibility:
	 * =========================================
	 * - Semaphore counter does not matter to Posix
	 * - In Posix semaphore there is no option to timeout on pending (opposite to uC/OS)
	 * - Posix semaphore always block on pending (in uC/OS there is an option to not block,
	 * 		but to return an error instead
	 * */
	(void) OSSemPend(sem, (OS_TICK)0, OS_OPT_PEND_BLOCKING, (CPU_TS)0, &err);

	if(err == OS_ERR_NONE)
		return(SEM_WAIT_SUCESS);
	else
		return(SEM_WAIT_FAIL);
}


int msc_sem_post(sem_t *sem)
{
	/* uC/OS-III specific types */
	OS_ERR   err;

	if(sem == 0)
		return SEM_POST_FAIL;

	/* Posix to uC/OS-III semaphore compatibility:
	 * =========================================
	 * - Semaphore counter does not matter to Posix
	 * - Only the highest task waiting for semaphore is released (uC/OS has the option
	 * 		to release all pending tasks waiting for the semaphore
	 * */
	(void)OSSemPost(sem, OS_OPT_POST_1, &err);

	if(err == OS_ERR_NONE)
		return(SEM_POST_SUCESS);
	else
		return(SEM_POST_FAIL);
}


int msc_sem_close(sem_t *sem)
{
	return -1;
}

