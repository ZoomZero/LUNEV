#include "maintainance.h"

int main()
{
  int key = 0;
  int bytes_read = 0;
  int res = 0;
  int sem_id = 0;
  int shm_id = 0;
  int cur_elem = 0;
  int * shm_int = NULL;

  void * shm = NULL;
  struct sembuf * sem_buf = NULL;

  key = ftok(FILE_5IPC, PROJ_ID);
  if (key < 0)
  {
    printf("Key cannot be created\n");
    exit(EXIT_FAILURE);
  }

  sem_id = semget(key, SEM_NUM, IPC_CREAT | 0666);
  if (sem_id < 0)
  {
    printf("Semaphores id not created\n");
    exit(EXIT_FAILURE);
  }

  shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  if (shm_id < 0)
  {
    printf("Shared memory id not created\n");
    exit(EXIT_FAILURE);
  }

  shm = shmat(shm_id, NULL, 0);
  if (shm == (void*)(0))
  {
    printf("Cannot get shared memory adress\n");
    exit(EXIT_FAILURE);
  }
  shm_int = (int*) shm;

  sem_buf = (struct sembuf *) calloc(SEM_NUM, sizeof(*sem_buf));

  SemBufChange(sem_buf, WAIT_READER, 0, 0, &cur_elem);
  SemBufChange(sem_buf, WAIT_READER, 1, SEM_UNDO, &cur_elem);
  SemBufChange(sem_buf, OCCUPY_READER, 0, 0, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  SemBufChange(sem_buf, WAIT_WRITER, -1, 0, &cur_elem);
  SemBufChange(sem_buf, WAIT_WRITER, 1, 0, &cur_elem);
  SemBufChange(sem_buf, OCCUPY_WRITER, 1, SEM_UNDO, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  do
  {
      SemBufChange(sem_buf, WAIT_WRITER, -1, IPC_NOWAIT, &cur_elem);
      SemBufChange(sem_buf, WAIT_WRITER, 1, 0, &cur_elem);
      SemBufChange(sem_buf, IS_FULL, -1, 0, &cur_elem);
      SemBufChange(sem_buf, MUTEX, -1, SEM_UNDO, &cur_elem);
      res = SendSemBuf(sem_buf, sem_id, &cur_elem);

      if (res != 0)
      {
        printf("Reader isn't workong properly\n");
        exit(EXIT_FAILURE);
      }

      bytes_read = *shm_int;
      if (bytes_read > 0) write(STDOUT_FILENO, (char*) shm + sizeof(int), bytes_read);
      else if (bytes_read == 0)
      {
        printf("finished reading from shm mem\n");
        SemBufChange(sem_buf, MUTEX, 1, SEM_UNDO, &cur_elem);
        SendSemBuf(sem_buf, sem_id, &cur_elem);

        break;
      }
      else
      {
        printf("Negative bytes' number\n");
        exit(EXIT_FAILURE);
      }

      SemBufChange(sem_buf, MUTEX, 1, SEM_UNDO, &cur_elem);
      SendSemBuf(sem_buf, sem_id, &cur_elem);
  } while(bytes_read > 0);

  free(sem_buf);

  return 0;
}
