#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <climits>
#include <unistd.h>


struct msg_buf
{
  long msg_type;
};

void send_msg(int qid, int msgtype, int size)
{
    msg_buf msg;

    msg.msg_type = msgtype;

    errno = 0;
    if (msgsnd(qid, (void*)&msg, size, 0) == -1) //IPC_NOWAIT??
    {
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent: %ld\n", msg.msg_type);
}

void get_msg(int qid, int msgtype, int size)
{
    msg_buf msg;

    errno = 0;
    if (msgrcv(qid, (void*)&msg, size, msgtype, MSG_NOERROR) == -1)  //| IPC_NOWAIT??
    {
        if (errno != ENOMSG)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("No message available for msgrcv()\n");
    }
    else printf("message received: %ld\n", msg.msg_type);
}

int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("No child count\n");
    exit(EXIT_FAILURE);
  }

  char *endptr = NULL;
  long long number = strtoll(argv[1], &endptr, 10);
  errno = 0;

  if(*endptr != '\0' || endptr == argv[1])
  {
    printf("Not a number in argv[1]\n");
    exit(EXIT_FAILURE);
  }

  if (((number == LONG_MIN || number == LONG_MAX) && errno == ERANGE) || (errno != 0 && number == 0) || number < 0)
  {
    printf("Wrong number\n");
    exit(EXIT_FAILURE);
  }

  int msg_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

  send_msg(msg_id, 1, 0);

  int i = 1;
  int c_num = 0;
  for( ; i <= number; i++)
  {
    pid_t c_pid = fork();

    if (c_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (c_pid == 0) break;
  }

  if (i < number)
  {
    get_msg(msg_id, i, 0);
    printf("%d\n", i);
    //fflush(stdout);

    send_msg(msg_id, i+1, i);

    exit(0);
  }
  else
  {
      get_msg(msg_id, number, 0);

      int ret_ctl = msgctl(msg_id, IPC_RMID, NULL);
      if (ret_ctl != 0)
      {
        perror("Cant delete msg queue\n");
        exit(EXIT_FAILURE);
      }
  }

  return 0;
}
