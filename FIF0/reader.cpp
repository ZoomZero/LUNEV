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

  int pipe_fd = FileOpen(PIPENAME, O_RDONLY);// | O_NONBLOCK);
  printf("opened %s, fd: %d\n", PIPENAME, pipe_fd);

  pid_t writer_pid;
  pid_t test_pid;
  ssize_t read_ret;

/*  int i = 0;
  for(; i < 3; i++)
  {
    printf("%d\n", i);
    read_ret = ReadFile(pipe_fd, &test_pid, sizeof(test_pid));
    printf("readed pid: %d\n read_ret: %ld\n", test_pid, read_ret);
    if (read_ret > 0)
    {
      writer_pid = test_pid;
      break;
    }

    //sleep(3);
  }

  if (i == 3)
  {
    printf("Cannot connect to get pid\n");
    exit(EXIT_FAILURE);
  }*/

  //DisableNONBLOCK(pipe_fd);

  ReadFile(pipe_fd, &writer_pid, sizeof(writer_pid));
  printf("writer_pid: %d\n", writer_pid);

  char * fifo_name = FifoName(writer_pid);
  printf("fifo_name = %s\n", fifo_name);
  FifoCreate(fifo_name, 0666);
  printf("created %s\n", fifo_name);
  int fifo_fd = FileOpen(fifo_name, O_WRONLY | O_NONBLOCK);
  printf("opened %s, fd: %d\n", fifo_name, fifo_fd);

  DisableNONBLOCK(fifo_fd);

  //ssize_t read_ret;
  ssize_t wrt_ret;

  do
  {
    read_ret = ReadFile(file_fd, buff, 256*sizeof(buff[0]));
    printf("readed from buf: %ld symbols\n", read_ret);
    printf("%s\n", buff);
    if (read_ret == 0) break;

    wrt_ret = WriteFile(fifo_fd, buff, read_ret*sizeof(buff[0]));
    printf("written to fifo: %ld symbols\n", wrt_ret);
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
