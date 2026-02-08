
#define CAROL_IMPLEMENTATION
#include "carol.h"
#include <stdio.h>

void carol_render(void)
{
	page index=page_begin("index.html");
		header("hello, Carol! fjapofgjpagjpagjpaogjpoadgjpadgjpadgjpadgjadpodgjadpogjapdgjadpgjadpogjadpgajpgadjgpoadjgpoadjgpaodgjpoadjgpoadjgpaodj",&index);
		para("it's working, isn't it?",&index);
		hyperlink("https://asgooffeeasme.neocities.org","Click Me!",&index);
	page_end(&index);
	return;
}

int main(int argc,char**argv)
{
	if((carol_init())==-1)
		return -1;
	return 0;
}
