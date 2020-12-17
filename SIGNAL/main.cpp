#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/prctl.h>

#define BUFSZ 256
#define PLog "ParentLog.txt"
#define CLog "ChildLog.txt"

FILE * p_log = fopen(PLog, "w+");
FILE * c_log = fopen(CLog, "w+");

static unsigned char byte = 0;
static unsigned char offset = 1;
static unsigned char buf[BUFSZ] = {};
static int err = 0;
static int i = 0;
static pid_t c_pid = 0;
static pid_t p_pid = 0;

int child(pid_t ppid, char * filename, sigset_t block_set, sigset_t empty_set);
int parent(pid_t pid, sigset_t block_set, sigset_t empty_set);
void ErrorCheck(bool err, const char* description);
void ParentIsDead(int signum);
void ParentIsAlive(int signum);
void ChildIsDead(int signum);
void GotOne(int signum);
void GotZero(int sugnum);
void ByteToBuf(unsigned char * buff, char c, size_t size);
void WriteToConsole(unsigned char * buff, size_t size);

int main(int argc, char *argv[])
{
	ErrorCheck(argc != 2, "No file to read");

	sigset_t empty_set;
	err = sigemptyset(&empty_set);
	ErrorCheck(err, "Error creating empty set");

	sigset_t block_set;
	err = sigemptyset(&block_set);
	ErrorCheck(err < 0, "Error initializng block set in parent");
	err = sigaddset(&block_set, SIGUSR1);
	ErrorCheck(err < 0, "Error adding SIGUSR1 to block_set");
	err = sigaddset(&block_set, SIGUSR2);
	ErrorCheck(err < 0, "Error adding SIGUSR2 to block_set");
	err = sigaddset(&block_set, SIGCHLD);
	ErrorCheck(err < 0, "Error adding SIGCHLD to block_set");
	err = sigaddset(&block_set, SIGALRM);
	ErrorCheck(err < 0, "Error adding SIGALRM to block_set");
	err = sigprocmask(SIG_BLOCK, &block_set, NULL);
	ErrorCheck(err < 0, "Error changing sigmask of parent");

	p_pid = getpid();
	c_pid = fork();

	ErrorCheck(c_pid < 0, "Error while creating child");

  if (c_pid == 0)		  child(p_pid, argv[1], block_set, empty_set);
  else                parent(c_pid, block_set, empty_set);

}

int child(pid_t ppid, char * filename, sigset_t block_set, sigset_t empty_set)
{
	err = prctl(PR_SET_PDEATHSIG, SIGTERM);
	ErrorCheck(err < 0, "Prctl error");

	if (getppid() != p_pid)
	{
		printf("Parent died");
		exit(EXIT_FAILURE);
	}

  int fd = open(filename, O_RDONLY);
	ErrorCheck(fd < 0, "File cannot be open");
	fprintf(c_log, "Tranferring file: %s\n", filename);

	struct sigaction p_dead;
	p_dead.sa_handler = ParentIsDead;
	p_dead.sa_flags = 0;
	err = sigfillset(&p_dead.sa_mask);
	ErrorCheck(err < 0, "Error initalizing p_dead mask");
	err = sigaction(SIGTERM, &p_dead, NULL);
	ErrorCheck(err < 0, "Error creating handler if parent is dead");

	struct sigaction p_alive;
	p_alive.sa_handler = ParentIsAlive;
	p_alive.sa_flags = 0;
	err = sigfillset(&p_alive.sa_mask);
	ErrorCheck(err < 0, "Error initalizing p_alive mask");
	err = sigaction(SIGUSR1, &p_alive, NULL);
	ErrorCheck(err < 0, "Error creating handler if parent is alive");

	while (read(fd, &byte, 1) > 0)
	{
		fprintf(c_log, "byte = %d \\ %c \n", byte, byte);

		for (offset = 1; offset > 0; offset = offset << 1)
		{
			fprintf(c_log, "offset = %d\n", offset);
			fprintf(c_log, "sending byte & offset = %d\n", offset & byte);

			if (offset & byte)    kill(ppid, SIGUSR1);
			else                  kill(ppid, SIGUSR2);
			//alarm(1);
			sigsuspend(&empty_set);
		}

	}

	fprintf(c_log, "thats all\n");

  close(fd);

  exit(EXIT_SUCCESS);
}

int parent(pid_t pid, sigset_t block_set, sigset_t empty_set)
{
	struct sigaction c_dead;
	c_dead.sa_handler = ChildIsDead;
	c_dead.sa_flags = 0;
	err = sigfillset(&c_dead.sa_mask);
	ErrorCheck(err < 0, "Error initalizing c_dead mask");
	err = sigaction(SIGCHLD, &c_dead, NULL);
	ErrorCheck(err < 0, "Error creating handler if child is dead");

	struct sigaction got_zero;
	got_zero.sa_handler = GotZero;
	got_zero.sa_flags = 0;
	err = sigfillset(&got_zero.sa_mask);
	ErrorCheck(err < 0, "Error initalizing got_zero mask");
	err = sigaction(SIGUSR2, &got_zero, NULL);
	ErrorCheck(err < 0, "Error creating handler if got bit = 0");

	struct sigaction got_one;
	got_one.sa_handler = GotOne;
	got_one.sa_flags = 0;
	err = sigfillset(&got_one.sa_mask);
	ErrorCheck(err < 0, "Error initalizing got_one mask");
	err = sigaction(SIGUSR1, &got_one, NULL);
	ErrorCheck(err < 0, "Error creating handler if got bit = 1");

  while(1)
  {
		sigsuspend(&empty_set);

		fprintf(p_log, "byte now = %d \\ %c\n", byte, byte);

		if (offset == 0)
		{
			fprintf(p_log, "final byte = %d \\ %c\n", byte, byte);

			ByteToBuf(buf, byte, i);

			offset = 1;
			byte = 0;
		}
  }

	exit(EXIT_SUCCESS);
}

void ParentIsDead(int signum)
{
	fprintf(p_log, "Parent is dead, sorry!\n");
	exit(EXIT_FAILURE);
}

void ParentIsAlive(int signum)
{
	//alarm(0);
}

void ChildIsDead(int signum)
{
	WriteToConsole(buf, i);
	exit(EXIT_SUCCESS);
}

void GotOne(int signum)
{
	fprintf(p_log, "offset in GotOne = %d\n", offset);

	byte += offset;
	offset = offset << 1;

	kill(c_pid, SIGUSR1);
}

void GotZero(int sugnum)
{
	fprintf(p_log, "offset in GotZero = %d\n", offset);

	offset = offset << 1;

	kill(c_pid, SIGUSR1);
}

void ByteToBuf(unsigned char * buff, char c, size_t size)
{
	buff[i++] = c;

	fprintf(p_log, "i = %d\n", i);

	if(i == BUFSZ)
	{
		WriteToConsole(buff, size);
		i = 0;
		memset(buff, 0, size);
	}
}

void WriteToConsole(unsigned char * buff, size_t size)
{
	err = write(STDOUT_FILENO, buff, size);
	ErrorCheck(err < 0, "Error writing to console");

	fflush(stdout);
}

void ErrorCheck(bool err, const char * description)
{
	if(err)
	{
		printf("%s\n", description);
		exit(EXIT_FAILURE);
	}
}
