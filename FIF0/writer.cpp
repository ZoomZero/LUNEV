#include "maintainance.h"

int main(int argc, char const *argv[])
{
  char * buff = BuffAlloc(256, 'c');
  FifoCreate(PIPENAME, 0666);
  printf("created %s\n", PIPENAME);
  int pipe_fd = FileOpen(PIPENAME, O_WRONLY);// | O_NONBLOCK);
  printf("openeded %s, fd: %d\n", PIPENAME, pipe_fd);

  //DisableNONBLOCK(pipe_fd);

  pid_t my_pid = getpid();
  printf("my_pid: %d\n", my_pid);
  WriteFile(pipe_fd, &my_pid, sizeof(my_pid));

  char * fifo_name = FifoName(my_pid);
  //sleep(30);                                       // если один процесс умрет, то второй тут сломется на опене
  FifoCreate(fifo_name, 0666);
  printf("created %s\n", fifo_name);
  int fifo_fd = FileOpen(fifo_name, O_RDONLY | O_NONBLOCK);
  printf("opened %s, fd: %d\n", fifo_name, fifo_fd);

  DisableNONBLOCK(fifo_fd);

  ssize_t read_ret;
  int i = 0;
  for(; i < 3; i++)
  {
    printf("%d\n", i);
    read_ret = ReadFile(fifo_fd, buff, 256*sizeof(buff[0]));
    printf("readed: %ld symbols\n", read_ret);
    if (read_ret > 0)
    {
      PrintText(buff, read_ret);
      break;
    }

    sleep(3);
  }

  if (i == 3)
  {
    printf("Cannot connect to FIFO named %s\n", fifo_name);
    exit(EXIT_FAILURE);
  }

  do
  {
    read_ret = ReadFile(fifo_fd, buff, 256*sizeof(buff[0]));
    if (read_ret == 0) break;
    printf("readed: %ld symbols\n", read_ret);
    PrintText(buff, read_ret);
    printf("read_ret = %ld\n", read_ret);
  } while (read_ret > 0);

  close(pipe_fd);
  close(fifo_fd);
  remove(PIPENAME);
  remove(fifo_name);
  free(buff);
  free(fifo_name);

  return 0;
}
