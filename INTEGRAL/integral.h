#pragma once

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#define SUCCESS 0
#define ERROR -1
#define MAX_FILE_LENGTH 256
#define DX 0.0000001
#define CPU_ONLINE_FILE "/sys/devices/system/cpu/online"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

double integrate(unsigned int cpu_count, double a, double b);
long string_to_int(char * str);
void ErrorCheck(bool err, const char * description);
