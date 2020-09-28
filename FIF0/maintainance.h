#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#define PIPENAME "/tmp/my_pype"

int FileOpen(const char * filename, int flags)
{
  int fd;
  if ((fd = open(filename, flags)) < 0)
  {
    printf("Cannot open file: %s", filename);
    exit(EXIT_FAILURE);
  }
  return fd;
}

ssize_t ReadFile(int fd, void *buf, size_t count)
{
  ssize_t read_ret;
  if ((read_ret = read(fd, buf, count)) <= 0)
  {
    printf("Cannot read from file descriptor: %d\n", fd);
    exit(EXIT_FAILURE);
  }
  return read_ret;
}

ssize_t WriteFile(int fd, const void *buf, size_t count)
{
  ssize_t wrt_ret;
  if ((wrt_ret = write(fd, buf, count)) <= 0)
  {
    printf("Cannot write to file descriptor: %d\n", fd);
    exit(EXIT_FAILURE);
  }

  return wrt_ret;
}

void FifoCreate(const char * fifoname, mode_t mode)
{
  (void)umask(0);
  if (mkfifo(fifoname, mode) < 0 && errno != EEXIST)
  {
    printf("Cannot create FIFO: %s\n (not EEXIST)", fifoname);
    exit(EXIT_FAILURE);
  }
}

template <typename T> T* BuffAlloc (int size, T type)
{
  T* buff = (T*)calloc(size, sizeof(type));
  if (buff == NULL)
  {
    printf("Cannot allocate buffer\n");
    exit(EXIT_FAILURE);
  }
  return buff;
}

char * FifoName(pid_t pid)
{
  char * buff = BuffAlloc(20, 'c');
  memset(buff, '\0', 20);

  sprintf(buff, "%d", pid);
  strcat(buff, ".p");

  return buff;
}

ssize_t PrintText(char * buff, ssize_t buf_sz)
{
  assert(buff != NULL);

  ssize_t wrt_ret = WriteFile(STDOUT_FILENO, buff, buf_sz*sizeof(buff[0]));

  return wrt_ret;
}
