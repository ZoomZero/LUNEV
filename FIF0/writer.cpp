#include "maintainance.h"

int main(int argc, char const *argv[])
{
  char * buff = BuffAlloc(256, 'c');
  FifoCreate(PIPENAME, 0666);
  printf("created %s\n", PIPENAME);
  int pipe_fd = FileOpen(PIPENAME, O_WRONLY);
  printf("openeded %s, fd: %d\n", PIPENAME, pipe_fd);

  pid_t my_pid = getpid();
  printf("my_pid: %d\n", my_pid);
  WriteFile(pipe_fd, &my_pid, sizeof(my_pid));

  char * fifo_name = FifoName(my_pid);
  FifoCreate(fifo_name, 0666);
  printf("created %s\n", fifo_name);
  int fifo_fd = FileOpen(fifo_name, O_RDONLY | O_NONBLOCK);
  printf("opened %s, fd: %d\n", fifo_name, fifo_fd);

  ssize_t read_ret = true;
  sleep(2);
  while (read_ret > 0)
  {
    read_ret = ReadFile(fifo_fd, buff, 256*sizeof(buff[0]));
    printf("readed: %ld symbols\n", read_ret);
    PrintText(buff, read_ret);
  }


  remove(PIPENAME);
  remove(fifo_name);
  close(pipe_fd);
  close(fifo_fd);
  free(buff);
  free(fifo_name);

  return 0;
}
