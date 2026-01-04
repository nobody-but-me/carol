
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void loop(void){
	header("Welcome, Carol.");
	paragraph("welcome to Carol, the C static website generator (now really generating html files)!");
	hyperlink("Click Me!","https://asgooffeeasme.neocities.org");
	paragraph("another paragraph!");
	image("https://asgooffeeasme.neocities.org/assets/writing-room-image-If0Uf6ww.webp");// TODO: remove hotlinking
	return;
}

int main(int argc,char**argv){
	const unsigned int PORT=4242;
	if((init(PORT))==-1)
		return -1;
	return 0;
}
