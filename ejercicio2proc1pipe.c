/* -----------------------------------------------------------	*/
/*  Ejerc: comunicar 2 procesos con pipe + redirec. de salida	*/
/* -----------------------------------------------------------	*/
/*				Programar: "ls -l | wc > file.txt"	 			*/
/* -----------------------------------------------------------	*/
/*  1.-   ls -l		listar información archivos					*/
/*  2.-   wc		muestra num líneas, palabras y bytes		*/
/*  3.-   >			redireciona salida STDOUT a (file.txt)		*/
/* -----------------------------------------------------------	*/
/*  Se utilizarán: fork(), dup2(...), execlp(...), pipe(...)	*/
/* ----------------------------------------------------------- 	*/

#include <unistd.h>				// dup2
#include <stdio.h>
#include <stdlib.h>				// macros 'EXIT_SUCCESS / FAILURE' ...
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>			// wait
#include <sys/stat.h>
#include <fcntl.h>

#define READ_END 	0			// index pipe extremo lectura
#define WRITE_END 	1			// index pipe extremo escritura
#define FILE_NAME	"file.txt"  // archivo donde escribir

int	main	(void)
{
	int	fd1[2];					// array-pareja de fds del pipe
	int	fd2;					// fd para el file de escritura
	int exit_status;			// para el wait q hace el padre
	int	pid;					// id de proceso de los hijos

	pipe(fd1);					// crea pipe q comunica ls y wc
	pid = fork();				// padre e hijo ven el pipe
	if (pid < 0)
	{
		perror("error fork (ls)\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)							// hijo1 -> ejecuta "ls -l"
	{
		close(fd1[0]);							// hijo1 no lee, escribe lo q lee el padre
		dup2(fd1[1], STDOUT_FILENO);			// duplica fd del pipe y pasa a ser el OUT
		close(fd1[1]);							// cierra (porque se queda el duplicado)
		execlp("/bin/ls", "ls", "-l", NULL);	// exe: (ruta + cmd), cmd, option, no+argm)
		perror("error execlp (ls)\n");
		exit(EXIT_FAILURE);
	}
	else	// padre -> recibe resultado de ejecución de "ls -l"
	{
		close(fd1[1]);							// padre no escribe, lee lo q escribe el hijo1
		pid = fork();		// padre crea hijo2, q no hereda extremo de escritura (pq closed before)
		if (pid < 0)
		{
			perror("error fork (wc)\n");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)		// hijo2 -> ejecuta "wc", hereda extremo de lectura (pq quedó abierto)
		{
			fd2 = open(FILE_NAME, O_WRONLY);	// crea file si no existe
			dup2(fd1[0], STDIN_FILENO);			// duplica (fd del pipe -fd1- pasa a ser el IN)
			close(fd1[0]);						// tras duplicado, éste ya no hace falta y lo cierro
			dup2(fd2, STDOUT_FILENO);			// duplica dónde escribe (OUT cambia a fd2)
			close(fd2); 						// lo cierro (se conserva el duplicado)
			execlp("/usr/bin/wc", "wc", NULL);
			perror("error execlp (wc)\n");
			exit(EXIT_FAILURE);
		}
		else	// padre
		{
			close(fd1[0]);						// puesto que no lee, sólo escribe
			wait(&exit_status);
		}
		wait(&exit_status);
	}
	// un wait para cada hijo, pueden ponerse todos aquí
	return (0);
}

