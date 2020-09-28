#include "maintainance.h"

int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("No file to read\n");
    exit(EXIT_FAILURE);
  }

  char * buff = BuffAlloc(256, 'c');
  int file_fd = FileOpen(argv[1], O_RDONLY | O_NONBLOCK);

  FifoCreate(PIPENAME, 0666);
  printf("created %s\n", PIPENAME);

  int pipe_fd = FileOpen(PIPENAME, O_RDONLY);
  printf("opened %s, fd: %d\n", PIPENAME, pipe_fd);

  pid_t writer_pid;
  ReadFile(pipe_fd, &writer_pid, sizeof(writer_pid));
  printf("writer_pid: %d\n", writer_pid);

  char * fifo_name = FifoName(writer_pid);
  printf("fifo_name = %s\n", fifo_name);
  FifoCreate(fifo_name, 0666);
  printf("created %s\n", fifo_name);
  int fifo_fd = FileOpen(fifo_name, O_WRONLY | O_NONBLOCK);
  printf("opened %s, fd: %d\n", fifo_name, fifo_fd);

  ssize_t read_ret = true;
  ssize_t wrt_ret;
  while (read_ret > 0)
  {
    read_ret = ReadFile(file_fd, buff, 256*sizeof(buff[0]));
    printf("readed from buf: %ld symbols\n", read_ret);
    wrt_ret = WriteFile(fifo_fd, buff, read_ret*sizeof(buff[0]));
    printf("written to fifo: %ld symbols\n", wrt_ret);
  }

  close(file_fd);
  close(pipe_fd);
  close(fifo_fd);
  free(buff);
  free(fifo_name);

  return 0;
}
