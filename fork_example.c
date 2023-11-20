	/*  fork(). Parte II. fork, wait, exit_status y múltiples procesos hijo */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_CHILD 5

/* pid_t	ft_fork(void); */ // ft para crear un nuevo proceso, copia del padre

/* pid_t	ft_wait(int *wstatus); */ // la llama el padre para esperar a que un hijo termine

/* WEXITSTATUS(wstatus) */ // macro (returns the exit_status of the child) This macro should be employed only if WIFEXITED returned true (WIFEXITED returns true if the child terminated normally, that is, by calling 'exit' or _exit, or by returning from main().

int	do_something(void)
{
	int	ret;

	srand(getpid());	// seed value to the random number
	ret = (rand() % 256);	// random value calculated (from 0 to 255)
	printf("CHILD: PID = %d, random value calculated = %d \n", getpid(), ret);
	return (ret);
}

int	main(void)
{
	pid_t	pidC;
	int	status;

	for (int i = 0; i < NUM_CHILD; i++)
	{
		pidC = fork();
		if (pidC > 0)
		{
			continue; // para que siga haciendo el bucle, creando hijos
		}
		else if (pidC == 0)
		{
			exit(do_something()); // can be from 0 to 255
		}
		else
		{
			printf("error\n");
		}
	}
	for (int i = 0; i < NUM_CHILD; i++)
	{
		pidC = wait(&status);
			printf("FATHER with PID = %d, CHILD with PID = %d ended, CHILD exit_status = %d\n", \
			getpid(), pidC, WEXITSTATUS(status));
	}
//	while (1) // the father do this (the children finish in their own 'if')
//	{
//		sleep(10);
//	}
	return (0);
}
