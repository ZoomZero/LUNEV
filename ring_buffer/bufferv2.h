#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define ERROR -1

#define MIN(a, b) ((a) > (b) ? (b) : (a))

typedef struct rb BUF;

struct rb
{
  int head, tail, tmp_head;
  int size;
  char data[0];
  union shadow
  {
    struct
    {
      int32_t head;
      int32_t wr_cnt;
    };
    int64_t shadow;
  };
};
