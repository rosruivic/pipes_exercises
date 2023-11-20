#include <unistd.h>				// dup2
#include <stdio.h>
#include <stdlib.h>				// macros 'EXIT_SUCCESS / FAILURE' ...
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>			// wait
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @brief  ** CASO 3: EJECUTO UN COMANDO Y REDIREC. entrada y SALIDA A FILE  **
 * 					( infile < cmd > outfile )
 * @param argc 
 * @param argv 	1) infile, abs_cmd, cmd, outfile
 * @return int 
 */
int	main (int argc, char **argv)
{
	int	exit_status;
	int	pid;
	int	fd_infile;
	int	fd_outfile;

	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	else if (pid == 0)
	{
		fd_infile = open(argv[1], O_RDONLY);
		if (fd_infile < 0)
		{
			printf("error apertura fichero entrada: fd = %d\n", fd_infile);
			exit(EXIT_FAILURE);
		}
		dup2(fd_infile, STDIN_FILENO);
		close(fd_infile);
		fd_outfile = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0644);
		if (fd_outfile < 0)
		{
			printf("error apertura fichero salida: fd = %d\n", fd_outfile);
			exit(EXIT_FAILURE);
		}
		dup2(fd_outfile, STDOUT_FILENO);
		close(fd_outfile);
		execlp(argv[2], argv[2], NULL);
		exit(EXIT_FAILURE);
	}
	else
		wait(&exit_status);
	return(0);
}

/**
 * @brief   ** CASO 2: EJECUTO UN COMANDO Y REDIREC. LA SALIDA A FILE ( > )		**
 * 				fd_outfile = open("file.txt", O_CREAT | O_TRUNC | O_RDWR, 0644);
 * 			** O REDIRECCIONO LA SALIDA A FILE EN MODO APPEND ( >> )			**
 * 				fd_outfile = open("file.txt", O_CREAT | O_APPEND | O_RDWR, 0644);
 * @param argc 
 * @param argv 
 * @return int 
 */
/* int	main(int argc, char **argv)
{
	int	exit_status;
	int	pid;			// PQ HACE FALTA HACER UN FORK
	int	fd_outfile;		// PQ HACE FALTA REDIRIGIR LA SALIDA A OTRO fd que no sea el STDOUT=1

	pid = fork();
	if (pid < 0) // error del fork, no se ha creado el proceso hijo
		exit(EXIT_FAILURE);
	else if(pid == 0) // HIJO
	{
		fd_outfile = open("file.txt", O_CREAT | O_APPEND | O_RDWR, 0644); // en modo reescritura, no append
		if (fd_outfile < 0) // error de apertura del archivo
		{
			printf("ERROR: fd %d\n", fd_outfile);
			exit(EXIT_FAILURE);
		}
		dup2(fd_outfile, STDOUT_FILENO); // STDOUT_FILENO pasa de 1 (std) al fd del file
		close(fd_outfile); // ya está duplicado, el original ya no sirve y lo cerramos
		execlp(argv[1], argv[1], argv[2], NULL); // uso este por facilidad ahora
		exit(EXIT_FAILURE);
	}
	else // PADRE
		wait(&exit_status);
	return (exit_status);
} */



/**
 * @brief   ** CASO 1: SOLO EJECUTAR UN COMANDO **

 * @return int  // con execve, exit_failure sí cambiaría si el cmd no existe
 */
/* int	main(int argc, char **argv)
{
	// necesito un fork para poder mandar a ejecutar un comando, que devuelve un EXIT_STATUS
	int	pid;
	int	exit_status;

	pid = fork();
	if (pid == 0)
	{ // n_argmts vble: ruta abs+cmd, cmd, argm1, argm2, ..., acaba en NULL 
		execlp(argv[1], argv[1], argv[2], NULL);
		exit(EXIT_FAILURE); // fallo del execlp, por eso llega a esta línea 
	}
	else if (pid > 0)
		wait(&exit_status);
	else // error del fork, no se ejecutó y no se creó el proceso hijo
		exit(EXIT_FAILURE);
	return (exit_status);
} */
