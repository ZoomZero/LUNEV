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
  int pipe_fd = FileOpen(PIPENAME, O_RDONLY);

  pid_t writer_pid = 0;
  ReadFile(pipe_fd, &writer_pid, sizeof(writer_pid));

  char * fifo_name = FifoName(writer_pid);
  FifoCreate(fifo_name, 0666);
  int fifo_fd = FileOpen(fifo_name, O_WRONLY | O_NONBLOCK);

  DisableNONBLOCK(fifo_fd);

  ssize_t wrt_ret;
  ssize_t read_ret;

  do
  {
    errno = 0;
    read_ret = ReadFile(file_fd, buff, 256*sizeof(buff[0]));
    if (read_ret == 0) break;

    wrt_ret = WriteFile(fifo_fd, buff, read_ret*sizeof(buff[0]));

    if (wrt_ret <= 0 && errno == EPIPE)
    {
      printf("PIPE error\n");
      exit(EXIT_FAILURE);
    }
  } while (read_ret > 0);

  close(file_fd);
  close(pipe_fd);
  close(fifo_fd);
  free(buff);
  free(fifo_name);

  return 0;
}
