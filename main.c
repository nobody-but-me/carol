
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void carol_render(void){
	header("Welcome, Carol.");
	paragraph("welcome to Carol, the header-only C static website generator (now really generating html files)!");
	hyperlink("Click Me!","https://asgooffeeasme.neocities.org");
	paragraph("another paragraph! And below an image:");
	image("media/the_sailor.webp");
	return;
}

int main(int argc,char**argv){
	if((carol_init())==-1)
		return -1;
	return 0;
}
