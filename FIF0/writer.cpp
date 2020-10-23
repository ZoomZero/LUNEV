#include "maintainance.h"

int BUFSZ = 256;

int main(int argc, char const *argv[])
{
  //char * buff = BuffAlloc(256, 'c');
  char buff[BUFSZ];

  FifoCreate(PIPENAME, 0666);
  int pipe_fd = FileOpen(PIPENAME, O_WRONLY);

  pid_t my_pid = getpid();

  char * fifo_name = FifoName(my_pid);
  FifoCreate(fifo_name, 0666);
  int fifo_fd = FileOpen(fifo_name, O_RDONLY | O_NONBLOCK);

  WriteFile(pipe_fd, &my_pid, sizeof(my_pid));

  ssize_t read_ret;
  int i = 0;
  for(; i < 3; i++)
  {
    read_ret = ReadFile(fifo_fd, buff, 256*sizeof(buff[0]));

    if (read_ret > 0)
    {
      PrintText(buff, read_ret);
      break;
    }

    /*int count = read_ret;
    if (read_ret > 0) {
    write(STDOUT_FILENO, buff, count);
    break; }*/

    sleep(1);
  }

  if (i == 3)
  {
    printf("Cannot connect to FIFO named %s\n", fifo_name);
    exit(EXIT_FAILURE);
  }

  DisableNONBLOCK(fifo_fd);

  do
  {
    errno = 0;
    read_ret = ReadFile(fifo_fd, buff, BUFSZ);
    if (read_ret == 0) break;

    PrintText(buff, read_ret);

  } while (read_ret > 0);

  /*int count = 0;
  while (1) {
  count = read(fifo_fd, buff, BUFSZ);
  if (count == 0) {
    break;
  }
  write(STDOUT_FILENO, buff, count);
}*/

  close(pipe_fd);
  close(fifo_fd);
  remove(PIPENAME);
  remove(fifo_name);
  free(buff);
  free(fifo_name);

  return 0;
}
