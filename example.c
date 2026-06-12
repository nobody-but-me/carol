
#include <stdio.h>

#define CAROL_IMPLEMENTATION
#include "carol.h"

void carol_compose(void)
{
	add_media("media/the_sailor.png","the_sailor.png");

	page index;
	
	page_conf configuration;
	create_configuration("Carol Website","index","index",&configuration);
	
	page_begin(&index,&configuration);
        header("Hello, Carol!");
		para("Congrats, you just generated your first HTML files with Carol! Now you can experiment a little, play around, add some more elements!");
		hyperlink("https://github.com/nobody-but-me/carol.git","Click me to go to the Carol repository");
		div_begin();
			para("and this paragraph should be inside a div element");
		div_end();
		link_image("https://github.com/nobody-but-me/carol.git","the_sailor.png","the crazy sailor");
    page_end();
    return;
}

int main(int argc,char**argv)
{
    if((carol_init())==-1)
        return -1;
    return 0;
}
