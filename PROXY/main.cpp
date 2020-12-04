#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>

#define BUFSZ 1000

void ErrorCheck(bool err, const char * description);
void BuffAlloc(struct Buffer * buf, int size);
void SendBuf(int number, int read_fd, int write_fd);
void WrittenToBuf(struct Buffer * buf, int num);
void ReadedFromBuf(struct Buffer * buf, int num);

struct Buffer
{
    char * buf;
    int size;
    char * pos;
    int Avaliable;
};

struct Connection
{
  pid_t p_in;
  pid_t p_out;

  struct Buffer Buf;
  int read_fd;
  int write_fd;

  bool AvaliableToClose;
};

int main(int argc, char const *argv[])
{
  ErrorCheck(argc != 3, "Wrong number of arguments");

  int n = strtol(argv[1], NULL, 10);
  ErrorCheck(n < 1, "Wrong number of children");

  int file_fd = open(argv[2], O_RDONLY);
  ErrorCheck(file_fd < 0, "Cannot open file");

  struct Connection * connect = (struct Connection *) calloc(n + 1, sizeof(struct Connection));
  connect[0].read_fd = file_fd;
  connect[n].write_fd = STDOUT_FILENO;

  int size = 3;

  for (int i = n; i >= 0; i--)
  {
    BuffAlloc(&connect[i].Buf, size);
    size *= 3;
    connect[i].AvaliableToClose = 0;
  }

  int pipe_fd_p_to_c[2] = {};
  int pipe_fd_c_to_p[2] = {};

  pid_t c_pid = 0;
  int max_fd = 0;
  int error = 0;

  fd_set read_set;
  fd_set write_set;

  for (int i = 0; i < n; i++)
  {
    error = pipe(pipe_fd_c_to_p);
    ErrorCheck(error < 0, "Error creating pipe for child");

    error = pipe(pipe_fd_p_to_c);
    ErrorCheck(error < 0, "Error creating pipe for parent");

    c_pid = fork();
    ErrorCheck(c_pid < 0, "Bad fork");

    if (c_pid == 0)
    {
      //printf("we are in child\n");
      for(int j = 0; j < i; j++)
      {
          error = close(connect[j].read_fd);
          ErrorCheck(error < 0, "Error closing read_fd in child");
          error = close(connect[j].write_fd);
          ErrorCheck(error < 0, "Error closing write_fd in child");
      }

      error = close(pipe_fd_p_to_c[1]);
      ErrorCheck(error < 0, "Error closing writing fd in child for parent");
      error = close(pipe_fd_c_to_p[0]);
      ErrorCheck(error < 0, "Error closing reading fd in child for parent");

      SendBuf(i, pipe_fd_p_to_c[0], pipe_fd_c_to_p[1]);

      return 0;
    }
    else
    {
      error = close(pipe_fd_p_to_c[0]);
      ErrorCheck(error < 0, "Error closing reading fd in parent for child");
      error = close(pipe_fd_c_to_p[1]);
      ErrorCheck(error < 0, "Error closing writing fd in parent for child");

      //printf("we are in parent\n");
      connect[i].p_out = c_pid;
      connect[i+1].p_in = c_pid;

      connect[i].write_fd = pipe_fd_p_to_c[1];
      connect[i+1].read_fd = pipe_fd_c_to_p[0];

      fcntl(pipe_fd_p_to_c[1], F_SETFL, O_NONBLOCK);

      if (pipe_fd_p_to_c[1] > max_fd) max_fd = pipe_fd_p_to_c[1];
      if (pipe_fd_c_to_p[0] > max_fd) max_fd = pipe_fd_c_to_p[0];
    }

  }

  struct timespec timeout = {};

  sigset_t fullset;
  sigfillset(&fullset);

  int readyFDnum = 0;
  int tail = -1;
  int bytes_read = 0;

  while (tail < n)
  {
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    for(int i = tail + 1; i <= n; i++)
    {
      if(connect[i].Buf.Avaliable == 0) FD_SET(connect[i].read_fd, &read_set);
      else                              FD_SET(connect[i].write_fd, &write_set);
    }

    readyFDnum = pselect(max_fd + 1, &read_set, &write_set, NULL, NULL, &fullset);
    //printf("Number of ready fd = %d\n", readyFDnum);

    for(int i = tail + 1; i <= n; i++)
    {
      if(FD_ISSET(connect[i].read_fd, &read_set))
      {
        if (connect[i].Buf.Avaliable == 0)
        {
          bytes_read = read(connect[i].read_fd, connect[i].Buf.pos, connect[i].Buf.size);
          ErrorCheck(bytes_read < 0, "Reading error");

          ReadedFromBuf(&connect[i].Buf, bytes_read);

          if (bytes_read == 0) connect[i].AvaliableToClose = 1;
        }
      }

      if(FD_ISSET(connect[i].write_fd, &write_set))
      {
        if (connect[i].Buf.Avaliable > 0)
        {
          bytes_read = write(connect[i].write_fd, connect[i].Buf.pos, connect[i].Buf.Avaliable);
          ErrorCheck(bytes_read < 0, "Writing error");

          WrittenToBuf(&connect[i].Buf, bytes_read);
        }
      }

      if(connect[i].AvaliableToClose && connect[i].Buf.Avaliable == 0)
      {
        error = close(connect[i].read_fd);
        ErrorCheck(error < 0, "Error while closing read_fd in parent");
        error = close(connect[i].write_fd);
        ErrorCheck(error < 0, "Error while closing write_fd in parent");

        tail = i;

        //printf("tail = %d\n", tail);
      }
    }
  }

  for(int i = 0; i <= n; i++)
  {
    free(connect[i].Buf.buf);
  }
  free(connect);

  return 0;
}

void BuffAlloc(struct Buffer * buf, int size)
{
  buf->buf = (char *)calloc(size, sizeof(char));
  buf->size = size;
  buf->pos = buf->buf;
  buf->Avaliable = 0;
}

void WrittenToBuf(struct Buffer * buf, int num)
{
  ErrorCheck(buf->Avaliable < num, "Writing mote than available");
  buf->Avaliable -= num;

  if (buf->Avaliable > 0) buf->pos += num;
  else                    buf->pos = buf->buf;
}

void ReadedFromBuf(struct Buffer * buf, int num)
{
  ErrorCheck(num > buf->size, "Readed more than buf size");
  buf->Avaliable = num;
  buf->pos = buf->buf;
}

void SendBuf(int number, int read_fd, int write_fd)
{
  int error = 0;
  int bytes_read = 1;
  char * buf = (char*) calloc(BUFSZ, sizeof(char));

  while (bytes_read > 0)
  {

    bytes_read = read(read_fd, buf, BUFSZ);
    ErrorCheck(error < 0, "Error reading from read_fd");

    bytes_read = write(write_fd, buf, bytes_read);
    ErrorCheck(error < 0, "Error writing to write_fd");
  }

  error = close(read_fd);

  error = close(write_fd);

  free(buf);
}


void ErrorCheck(bool err, const char * description)
{
	if(err)
	{
		printf("%s\n", description);
		exit(EXIT_FAILURE);
	}
}
