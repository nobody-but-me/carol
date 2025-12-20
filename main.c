
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void loop(void){
	header("Welcome, Carol.");
	paragraph("welcome to Carol, the C static website generator!");
	header("hello again!");
	paragraph("that's so freaking cool!");
	image("https://asgooffeeasme.neocities.org/assets/writing-room-image-If0Uf6ww.webp");
	return;
}

int main(int argc,char**argv){
	const int PORT=4242;
	if((init(PORT))==-1)
		return -1;
	return 0;
}
