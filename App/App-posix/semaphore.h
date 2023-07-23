/*** <semaphore.h> ***/

#ifndef  _SEMAPHORE_H_
#define  _SEMAPHORE_H_


#include "fcntl.h"
#include <os.h>

#define SEM_FAILED	0

typedef OS_SEM sem_t;	// POSIX to uC/OS-III semaphore mapping
typedef unsigned int	mode_t;


sem_t* sem_open (const char *name, int oflag, mode_t mode, unsigned int value);


#endif /* _SEMAPHORE_H_*/
