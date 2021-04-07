  #include "integral.h"

typedef struct ThreadData
{
    double a;
    double b;
    double result;
    cpu_set_t cpu_set;
} ThreadData;

typedef struct CpuInfo
{
    char * hypercpu_available;
    unsigned int available_hypercpus_num;
    cpu_set_t * hypercpu_sets;
} CpuInfo;

void ErrorCheck(bool err, const char * description)
{
  if(!err)
  {
  	printf("%s\n", description);
  	exit(EXIT_FAILURE);
  }
}

long string_to_int(char * str)
  {
  long input = 0;
  char * strptr = str;
  char * endptr = NULL;

  input = strtol(strptr, &endptr, 10);

  if (endptr == strptr || *endptr != '\0')
  {
  	return ERROR;
  }

  if (errno == ERANGE && (input == LONG_MAX || input == LONG_MIN))
  {
  	return ERROR;
  }

  return input;
}

int init_cpus(CpuInfo * info, unsigned int cpu_count)
{
  if (info == NULL)
    return ERROR;

  info->available_hypercpus_num = get_nprocs();
  unsigned int max_hypercpu = MAX(cpu_count, info->available_hypercpus_num);
  info->hypercpu_available = (char *) calloc(1, max_hypercpu);
  info->hypercpu_sets = (cpu_set_t *) calloc(max_hypercpu, sizeof(cpu_set_t));

  if ((info->hypercpu_available == NULL) || (info->hypercpu_sets == NULL))
    return -1;

  int cpu_fd = 0;
  int text_len = 0;
  char text[MAX_FILE_LENGTH] = {0};

  ErrorCheck((cpu_fd = open(CPU_ONLINE_FILE, O_RDONLY)), "Error opening cpu online file");
  ErrorCheck((text_len = read(cpu_fd, text + 1, MAX_FILE_LENGTH - 2)), "Error getting length of file");
  ErrorCheck(close(cpu_fd) == 0, "Error closing fd");

  text[0] = ',';
  text[text_len] = '\0';

  unsigned int commaIndex = 0;
  unsigned int block_start = 0;
  unsigned int block_end = 0;
  unsigned int hyphen_pos = 0;
  char * first_str = NULL;
  char * second_str = NULL;
  unsigned int first = 0;
  unsigned int second = 0;

  while (commaIndex < text_len) {
    block_start = commaIndex;
    commaIndex++;

    while (text[commaIndex] != ',' && text[commaIndex] != '\0')
    {
      commaIndex++;
    }

    block_end = commaIndex;

    for (hyphen_pos = block_start;
      (hyphen_pos < block_end) && (text[hyphen_pos] != '-'); hyphen_pos++) {}

    first_str = text + block_start + 1;
    second_str = text + hyphen_pos + 1;

    text[block_end] = '\0';
    text[hyphen_pos] = '\0';


    first = string_to_int(first_str);
    second = string_to_int(second_str);

    for (unsigned i = first; i <= second; i++)
    {
      info->hypercpu_available[i] = 1;
    }

  }

  return SUCCESS;
}

double function(double x)
{
  return x*x*x;
}

void * calculate(void* args)
{
  ThreadData * data = ((ThreadData*)args);

  cpu_set_t cpuset = ((ThreadData*)args)->cpu_set;
  pthread_t current_thread = pthread_self();
  double start = ((ThreadData*)args)->a;
  double finish = ((ThreadData*)args)->b;

  ErrorCheck(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset), "Wrong affinity");

  data->result = 0;

  double delta = DX;
  double x = start;

   for (; x < finish; x += delta) {
  	((ThreadData*)args)->result += function(x) * DX;
  }

  return NULL;
}

int cache_line_size()
{
  FILE * cache_info = fopen("/sys/bus/cpu/devices/cpu0/cache/index0/coherency_line_size", "r");

  if (cache_info == NULL)
  	return ERROR;

  int line_size = 0;
  fscanf(cache_info, "%d", &line_size);

  fclose(cache_info);

  return line_size;
}

int CreateThreadBuffers(int total, char ** data, int * buf_size)
{
  int line_size = 0;
  line_size = cache_line_size();

  int info_size = sizeof(ThreadData);

  if (info_size <= line_size)
  {
  	info_size = 2 * line_size;
  }
  else
  {
  	info_size = (info_size / line_size + 2) * line_size;
  }

  *buf_size = info_size;

  *data = (char *) malloc(total * info_size);

  if (*data == NULL)
  	return ERROR;


  memset(*data, 0, total * info_size);

  return SUCCESS;
}

double StartThreadCalc(CpuInfo * info, unsigned int cpu_count, double a, double b)
{
  if (info == NULL)
    return ERROR;

  double begin = a;
  double size = (double)(b - a) / (double) cpu_count;
  unsigned int max_hypercpu = MAX(cpu_count, info->available_hypercpus_num);
  pthread_t * threads = (pthread_t *) calloc(max_hypercpu, sizeof(pthread_t));

  if (!threads)
  	return ERROR;

  int buf_size = 0;
  char * data = NULL;
  CreateThreadBuffers(max_hypercpu, &data, &buf_size);

  for (unsigned i = 0; i < max_hypercpu; i++)
  {
  	ThreadData * cur = (ThreadData *) (data + i * buf_size);

  	if (i < cpu_count)
    {
  		*cur = (ThreadData) {
  			                    begin,
  			                    begin + size,
  			                    0,
  			                    info->hypercpu_sets[i % info->available_hypercpus_num],
            		            };
    }
    else
    {
      *cur = (ThreadData) {
  			                    a,
  			                    a + (b - a) / (double)(cpu_count),
  			                    0,
  			                    info->hypercpu_sets[i % info->available_hypercpus_num],
            		          };
    }

    begin += size;
    }

  for (unsigned i = 0; i < max_hypercpu; i++)
  {
  	pthread_create(&threads[i], NULL, &calculate, (data + i * buf_size));
  }

  double result = 0;
  for (unsigned i = 0; i < max_hypercpu; i++)
  {
  	pthread_join(threads[i], NULL);
  	ThreadData * cur = (ThreadData*) (data + i * buf_size);

  	if (i < cpu_count) result += cur->result;
  }

  free(data);

  return result;
}


double integrate(unsigned int cpu_count, double a, double b)
{
  double result = 0;
  CpuInfo * info = (CpuInfo *) calloc(1, sizeof(CpuInfo));

  ErrorCheck(init_cpus(info, cpu_count) == SUCCESS, "Wrong cpu initialization");
  result = StartThreadCalc(info, cpu_count, a, b);

  free(info->hypercpu_available);
  free(info->hypercpu_sets);
  free(info);

  return result;
}
