/**
 *
 * This program copies files using a pipe.
 *
 * Usage:
 *	filecopy <input file> <output file>
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#define INPUT 0
#define OUTPUT 1
#define READ_END 0
#define WRITE_END 1

void outputTimestamp();

int main(int argc, char *argv[])
{
	int rv;
	pid_t pid;
	int c;
	char rb[2], wb[2]; /* bytes for reading/writing */
	int ffd[2];		   /* file descriptor */

	/* Step1: Create pipe descriptor */
	int pipefd[2];

	/* Step2: set up the pipe */
	/* make sure your program closes file handles*/
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/* open the input file */
	ffd[INPUT] = open(argv[1], O_RDONLY); //ffd[0] holds address of input file (argv[1])

	if (ffd[INPUT] < 0)
	{
		fprintf(stderr, "Unable to open %s\n", argv[1]);
		return 1;
	}

	/* open the output file */
	ffd[OUTPUT] = open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); //ffd[1] holds address of output file (argv[2])

	if (ffd[1] < 0)
	{
		fprintf(stderr, "Unable to open %s\n", argv[2]);

		/* close the input file */
		close(ffd[INPUT]);

		return 1;
	}

	/* Step3: create the processes */
	/* read from the input file and write to the pipe */
	/* read from the pipe and write to the output file */
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{							  //child reads from pipe
		close(pipefd[WRITE_END]); //close unused write end

		while (read(pipefd[READ_END], rb, 1) > 0)
		{
			write(ffd[OUTPUT], rb, 1);
		}
		close(pipefd[READ_END]);
		outputTimestamp();
		_exit(EXIT_SUCCESS);
	}
	else
	{ //parent writes
		close(pipefd[READ_END]);
		while (read(ffd[INPUT], wb, 1) > 0)
		{
			write(pipefd[WRITE_END], wb, 1);
		}
		close(pipefd[WRITE_END]);
		wait(NULL);
		exit(EXIT_SUCCESS);
	}
}

void outputTimestamp()
{
	time_t mytime;
	mytime = time(NULL);
	char timeMsg[] = "Lab 4 read and write finished ";
	char curTime[26];
	strcpy(curTime, ctime(&mytime));
	strcat(timeMsg, curTime);
	write(STDOUT_FILENO, timeMsg, strlen(timeMsg));
	//printf("Lab 4 read and write finished %s.", ctime(&mytime));
	//printf wasn't outputting in k200
}

/*//cheat sheet to test code
rm K200cjb49469 K200cjb49469.c copy.txt
exit
//up, up, enter
//type in password
//up, up, enter
exit
//up, up, enter
//type in password
gcc K200cjb49469.c -o K200cjb49469
./K200cjb49469 input.txt copy.txt
cat copy.txt
*/