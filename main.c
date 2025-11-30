
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	printf("Hello, Carol.\n");
	const int PORT = 4242;
	if ((init(PORT)) == -1)
		return -1;
	return 0;
}
