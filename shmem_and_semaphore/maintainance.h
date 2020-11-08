#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_5IPC "file.txt"
#define PROJ_ID 1
#define DATA_SIZE 512
#define SHM_SIZE (sizeof(int) + DATA_SIZE)

enum sems
{
  WAIT_READER,
  WAIT_WRITER,
  OCCUPY_READER,
  OCCUPY_WRITER,
  MUTEX,
  IS_FULL,
  SEM_NUM,
};

int SemBufChange(struct sembuf * sem_buf, short int semnum, short int semop, short int semflag, int * cur_elem)
{
  sem_buf[*cur_elem].sem_num = semnum;
  sem_buf[*cur_elem].sem_op = semop;
  sem_buf[*cur_elem].sem_flg = semflag;
  (*cur_elem)++;

  return 0;
}

int SendSemBuf(struct sembuf * sem_buf, int sem_id, int * cur_elem)
{
  int sem_num = *cur_elem;
  *cur_elem = 0;

  int res = semop(sem_id, sem_buf, sem_num);

  return res;
}
