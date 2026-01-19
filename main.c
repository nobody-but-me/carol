
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void render(void){
	header("Welcome, Carol.");
	paragraph("welcome to Carol, the header-only C static website generator (now really generating html files)!");
	hyperlink("Click Me!","https://asgooffeeasme.neocities.org");
	paragraph("another paragraph! And below an image:");
//	image("https://asgooffeeasme.neocities.org/assets/writing-room-image-If0Uf6ww.webp");// TODO: remove hotlinking
	image("media/the_sailor.webp");
	return;
}

int main(int argc,char**argv){
//	const unsigned int PORT=4242;
//	if((init(PORT))==-1)
	if((init())==-1)
		return -1;
	return 0;
}
