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

  sem_id = semget(key, SEMS_NUM, IPC_CREAT | 0666);
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

  sem_buf = (struct sembuf *) calloc(SEMS_NUM, sizeof(*sem_buf));

  SemBufChange(sem_buf, 0, 0, 0, &cur_elem);
  SemBufChange(sem_buf, 0, 1, SEM_UNDO, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  SemBufChange(sem_buf, 4, 0, 0, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  semctl(sem_id, 3, SETVAL,1);

  SemBufChange(sem_buf, 0, 1, SEM_UNDO, &cur_elem);
  SemBufChange(sem_buf, 3, -1, 0, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  SemBufChange(sem_buf, 1, -2, 0, &cur_elem);
  SemBufChange(sem_buf, 1, 2, 0, &cur_elem);
  SemBufChange(sem_buf, 4, 1, SEM_UNDO, &cur_elem);
  SendSemBuf(sem_buf, sem_id, &cur_elem);

  do
  {

      SemBufChange(sem_buf, 2, -1, 0, &cur_elem);
      SendSemBuf(sem_buf, sem_id, &cur_elem);

      if (semctl(sem_id, 4, GETVAL) != 2) break;

      memset(shm_int, 0, DATA_SIZE);
      bytes_read = read(fd, shm_int + sizeof(int), DATA_SIZE - sizeof(int));
      *shm_int = bytes_read;
      memcpy((char*)shm + sizeof(int), buf, bytes_read);

      SemBufChange(sem_buf, 3, 1, 0, &cur_elem);
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
