/*** <semaphore.h> ***/

#ifndef  _SEMAPHORE_H_
#define  _SEMAPHORE_H_

#include "fcntl.h"
#include <os.h>

#define SEM_OPEN_FAIL	(0)

#define SEM_WAIT_SUCESS	(0)
#define SEM_WAIT_FAIL	(-1)

#define SEM_POST_SUCESS	(0)
#define SEM_POST_FAIL	(-1)


typedef OS_SEM sem_t;	// POSIX to uC/OS-III semaphore mapping
typedef unsigned int	mode_t;

/* Posix wrapper - Semaphore function prototypes */
sem_t* msc_sem_open (const char *name, int oflag, mode_t mode, unsigned int value);
int msc_sem_wait(sem_t *sem);
int msc_sem_post(sem_t *sem);
int msc_sem_close(sem_t *sem);


#endif /* _SEMAPHORE_H_*/
