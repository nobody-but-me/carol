
#define _GNU_SOURCE
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>
/*

const char*background="background-color: red;";

div_begin(background,&index);
div_end(&index);


header("hello, carol!","background-color: red;",&index);

header("hello, carol!",background,&index);
para("hello hello  hi",background,&index);

add_style_raw("h1","background-color: red;",&index);
add_style(CAROL_HEADERS,CAROL_BACKGROUND,"red",&index);

*/
void carol_compose(void)
{
    add_media("media/the_sailor.png","the_sailor.png");
    
    page_conf configuration={
		.style_path="index.css",
		.html_path="index.html",
		.title="Carol Website"
    };    
	page index;
	
	page_begin(&index,&configuration);
        header("Hello, Carol!");
		para("Congrats, you just generated your first HTML files with Carol! Now you can experiment a little, play around, add some more elements!");
		hyperlink("https://github.com/nobody-but-me/carol.git","Click me to go to the Carol repository");
		image("the_sailor.png","the sailor");
		div_begin();
			para("this paragraph should be inside a div element");
		div_end();
    page_end();
    return;
}

int main(int argc,char**argv)
{
    if((carol_init())==-1)
        return -1;
    return 0;
}
