#include "maintainance.h"

int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("No file to read\n");
    exit(EXIT_FAILURE);
  }

  int key = 0;
  int fd = 0;
  int res = 0;
  int bytes_read = 0;
  char buf[DATA_SIZE] = {};

  int sem_id = 0;
  int shm_id = 0;
  int cur_elem = 0;

  void * shm = NULL;
  struct sembuf * sem_buf = NULL;

  fd = open(argv[1], O_RDONLY);
  if (fd < 0)
  {
    printf("File cannot be open\n");
    exit(EXIT_FAILURE);
  }

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
  int *shm_int = (int *) shm;

  sem_buf = (struct sembuf *) calloc(SEM_NUM, sizeof(*sem_buf));

  SemBufChange(sem_buf, WAIT_WRITER, 0, 0, &cur_elem);
  SemBufChange(sem_buf, WAIT_WRITER, 1, SEM_UNDO, &cur_elem);
  SemBufChange(sem_buf, OCCUPY_WRITER, 0, 0, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  SemBufChange(sem_buf, WAIT_READER, -1, 0, &cur_elem);
  SemBufChange(sem_buf, WAIT_READER, 1, 0, &cur_elem);
  SemBufChange(sem_buf, OCCUPY_READER, 1, SEM_UNDO, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  SemBufChange(sem_buf, IS_FULL, 1, SEM_UNDO, &cur_elem);
  SemBufChange(sem_buf, IS_FULL, -1, 0, &cur_elem);
  SemBufChange(sem_buf, MUTEX, 0, 0, &cur_elem);
  SemBufChange(sem_buf, MUTEX, 1, SEM_UNDO, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  do
  {
      SemBufChange(sem_buf, WAIT_READER, -1, IPC_NOWAIT, &cur_elem);
      SemBufChange(sem_buf, WAIT_READER, 1, 0, &cur_elem);
      SemBufChange(sem_buf, IS_FULL, 0, 0, &cur_elem);
      SemBufChange(sem_buf, MUTEX, -1, SEM_UNDO, &cur_elem);
      res = SendSemBuf(sem_buf, sem_id, &cur_elem);

      if (res != 0)
      {
        printf("Writer isn't workong properly with res = %d\n", res);
        exit(EXIT_FAILURE);
      }

      bytes_read = read(fd, buf, DATA_SIZE);
      *shm_int = bytes_read;
      memcpy((char*)shm + sizeof(int), buf, bytes_read);

      SemBufChange(sem_buf, MUTEX, 1, SEM_UNDO, &cur_elem);
      SemBufChange(sem_buf, IS_FULL, 1, 0, &cur_elem);
      SendSemBuf(sem_buf, sem_id, &cur_elem);
  } while(bytes_read > 0);

  free(sem_buf);

  if (shmctl(shm_id, IPC_RMID, NULL) < 0)
  {
      printf("Error deattaching shared memory\n");
      exit(EXIT_FAILURE);
  }

  if (semctl(sem_id, 0, IPC_RMID) < 0)
  {
      printf("Error deleitng semaphores\n");
      exit(EXIT_FAILURE);
  }

  return 0;
}
