#include "bufferv2.h"

void alloc_space(BUF * rb, int size)
{
  int m_size = 0;
  union shadow sh, old_sh;
  do
  {
    sh.tmp_head = rb->shadow.shadow;
    old_sh.shadow = sh.shadow;
    m_size = MIN(size, rb->size-(sh.temp_head->rb->tail)); //значения из разделяемой памяти можно брать ОДИН раз
    sh.tmp_head += m_size;
    sh.wr_cnt++;
  } while(cmpexch(&rb->shadow.shadow, old_sh.shadow, sh.shadow) != old_sh.shadow);

}

void commit(BUF * rb)
{
  union shadow sh, old_sh;
  do
  {
    sh.shadow = rb->shadow.shadow;
    old_sh.shadow = sh.shadow;
    sh.wr_cnt--;
    if(sh.wr_cnt == 0)
    {
      rb->head = sh.tmp_head;
    }
  } while (cmpexch(&rb->shadow, old_sh.shadow, sh.shadow) != old_sh.shadow);
}

void rb_write()
{

}

void rb_read()
{

}

int cmpexch(int * addr, int a, int b)
{
  if (*addr == a)
  {
    *addr = b;
    return a;
  }
  return *addr;
}
