#include <errno.h>

void print_err(char *str, int line, int err_no)
{
	printf("%d, %s: %s\n", line, str, strerror(err_no));
	
	exit(-1);
}
