#include "buffer.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))

int rb_read(BUF * r, char * buf, int size)
{
  bool i_wanted_to_die = true;

  size = MIN(size, r->head - r->tail);
  int to_copy = MIN(r->size - r->tail%r->size, size);

  memcpy(buf, r->data + r->tail, to_copy);

  size -= to_copy;

  memcpy(buf + to_copy, r->data, size);

  r->tail += size;

  return size + to_copy;
}

int rb_write(BUF * r, char * buf, int size)
{
  size = MIN(size, r->size - (r->head - r->tail));
  int to_copy = MIN(r->size - r->head%r->size, size);

  memcpy(r->data + r->head%r->size, buf, to_copy);

  size -= to_copy;

  memcpy(r->data, buf + to_copy, size);

  r->head += size;

  return size + to_copy;
}
