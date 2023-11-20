#include <unistd.h>
#include <stdio.h>

int	g_var = 22;

int	main(void)
{
	pid_t	pidC;

	printf("** process PID = %d starts **\n", getpid());
	pidC = fork();

	printf("process PID = %d executing\n", getpid());
	if (pidC > 0) // padre
	{
		g_var = 44;
	}
	else if (pidC == 0) // hijo
	{
		g_var = 33;
	}
	else // == -1 -> error
	{
		printf("error creating child\n");
	}
	
	while (1)
	{
		sleep(3);
		printf("process PID = %d, var = %d executing\n", getpid(), g_var);
	}
	return 0;
}
