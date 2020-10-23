#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "reader.h"
#include "writer.h"


int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3)
  {
    printf("Uncorrect number of arguments\n");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "-r") == 0) reader(argv[2]);
  else if (strcmp(argv[1], "-w") == 0) writer();
  else
  {
    perror("WRONG ARGUMENT");
    exit(EXIT_FAILURE);
  }

  return 0;
}
