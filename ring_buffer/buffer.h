#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define ERROR -1

typedef struct rb BUF;

struct rb
{
  int head, tail;
  int size;
  char data[0];
};

int rb_read(BUF * ring, char * buf, int size);
int rb_write(BUF * ring, char * buf, int size);
