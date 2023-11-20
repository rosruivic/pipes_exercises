/* -----------------------------------------------------------	*/
/*  Ejerc: comunicar 2 procesos con pipe + redirec. de salida	*/
/* -----------------------------------------------------------	*/
/*		Programar: "ls -l | grep u | wc -l > file.txt"	 		*/
/* -----------------------------------------------------------	*/
/*  1.-	ls -l	(listar información archivos)					*/
/*  2.-	grep u	(busca la letra 'u')							*/
/*  3.-	wc -l	(muestra número de nuevas líneas				*/
/* 	4.- >		(probar a hacer yo el redirec. final)			*/
/* -----------------------------------------------------------	*/
/*  Se utilizarán: fork(), dup2(...), execlp(...), pipe(...)	*/
/* ----------------------------------------------------------- 	*/

// TODO: para tratar infinitos pipes, necesitaría un array de parejas de pipe-fds

#include <fcntl.h>
#include <unistd.h>				// dup2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>				// 'EXIT_FAILURE' / 'EXIT_SUCCESS' ...
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>			// wait

#define READ_END 	0			// index pipe extremo lectura
#define WRITE_END 	1			// index pipe extremo escritura
#define FILE_NAME	"file.txt"  // archivo donde escribir

int	main(void)
{
	int	fd1[2];					// para el pipe 1
	int	fd2[2];					// para el pipe 2
	int	exit_status;			// estado de los hijos
	int	pid;					// para el pid de los hijos

	pipe(fd1);					// primer pipe: comunica 'ls' con 'grep'
	pid = fork();				// creación del primer hijo
	if (pid < 0)
	{
		perror("fork (ls)\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)			// hijo 1 - hereda fds del padre
	{
		close(fd1[0]);			// cerrar extremo lectura, no necesario
	/* 	la salida STDOUT_FILENO se escriba tb en el pipe;
			el descriptor STDOUT_FILENO va a ser un duplicado de fd1[1]
			y, por lo tanto, fd1[1] ya no lo necesito y lo cierro:	 */
		dup2(fd1[1], STDOUT_FILENO); // duplico fd1[1] como nuevo STDOUT
		close(fd1[1]);	// una vez duplicado, éste no hace falta, lo cierro
		execlp("/bin/ls", "ls", "-l", NULL);
		perror("execlp (ls)\n");
		exit(EXIT_FAILURE); // AGREGA SALIDA PARA EVITAR QUE HIJO CONTINÚE
	}
	else // padre (hijos heredan fd abiertos, cerrar los no necesarios antes de fork)
	{
		close(fd1[1]);	// extremo de escritura primer pipe, no necesario ya
		pipe(fd2);		// segundo pipe: comunica 'grep' con 'wc'
		pid = fork();	// creación del segundo hijo
		if (pid < 0)
		{
			perror("fork (grep)\n");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)	// hijo 2 - hereda fds del padre
		{
			close(fd2[0]);	// cerrar extremo de lectura, no necesario
			dup2(fd1[0], STDIN_FILENO);
			close(fd1[0]);
			dup2(fd2[1], STDOUT_FILENO);
			close(fd2[1]);
			execlp("/usr/bin/grep", "grep", "u", NULL);
			perror("execlp (grep)\n");
		}
		else	// padre (cerrar fds que no sean necesarios antes de hacer fork)
		{
			close(fd1[0]);	// extremo lectura primer pipe, no necesario
			close(fd2[1]);	// extremo escritura segundo pipe, no necesario
			pid = fork();	// creación del tercer hijo
			if (pid < 0)
			{
				perror("fork (wd)\n");
				exit(EXIT_FAILURE);
			}
			else if (pid == 0)	// hijo 3 - hereda los fd del padre
			{
				dup2(fd2[0], STDIN_FILENO);
				close(fd2[0]);
				execlp("/usr/bin/wc", "wc", "-l", NULL);
				perror("execlp (wd)\n");
				exit(EXIT_FAILURE);
			}
			else // padre
			{
				close(fd2[0]);
				wait(&exit_status);
			}
			wait(&exit_status);
		}
		wait(&exit_status);
	}
	return (0);
}

