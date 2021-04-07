#include "integral.h"

int main (int argc, char * argv[])
{
	if (argc != 4)
  {
		printf("Wrong argument number\n");
		return 0;
	}

	int cpu_count = 0;
  double a = 0;
  double b = 0;
	double result = 0;

	ErrorCheck(cpu_count = string_to_int(argv[1]), "Wrong number of cpu for usage");
  a = string_to_int(argv[2]);
  b = string_to_int(argv[3]);

	result = integrate(cpu_count, a, b);
	printf("the result is %f\n", result);

	return 0;
}
