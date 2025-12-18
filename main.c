
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

int main(int argc,char**argv){
	printf("Welcome, Carol.\n");
	const int PORT=4242;
	if((init(PORT))==-1)
		return -1;
	
	header("hello");
	paragraph("welcome to Carol, the C static website generator!");
	
	render();
	return 0;
}
