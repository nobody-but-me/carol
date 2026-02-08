
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void carol_render(void)
{
	page index=page_begin("index.html",NULL);
		header("Hello, Carol!",&index);
		para("Congrats, you just generated your first HTML files with Carol! Now you can experiment a little, play around, create some more elements!",&index);
		hyperlink("https://github.com/nobody-but-me/carol.git","Click me to go to the Carol repository",&index);
	page_end(&index);
	return;
}

int main(int argc,char**argv)
{
	if((carol_init())==-1)
		return -1;
	return 0;
}
