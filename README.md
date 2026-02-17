
# Carol static website generator

Carol is a simple header-only static website generator written in the C programming language. Following the philosophy
of [j3s](https://j3s.sh/thought/my-website-is-one-binary.html) and [pigeonhole](https://iwillneverbehappy.neocities.org/site/about),
Carol generates HTML files based in one or more .c files, running them in localhost -- all within a single-binary.

Carol is meant to be simple, easy, fast, fun, reliable and straightfoward -- currently being just barebones :)

Carol is licensed under the GNU General Public License v2 (GPLv2), being any contributions more than welcome.

## Installation

Carol is a header-only library, so all you need to do is to include the .h file in your C/C++ project.

If you want to run Carol's example, you just need to run the follow commands in your terminal:

```

 > sh build.sh

# follow the instructions

 > sh run.sh

```

## Creating your website

Carol works with C functions that create HTML elements. Functions like ```header(const char*,page*)``` or ```para(const char*,page*)``` can be called inside
the pre-declared function ```carol_render(void)``` to generate the desired HTML file:

```c

void carol_render(void)
{
	/* first you need to create a page_conf struct, so you can set basic settings for your website */
	page_conf config = {
		.style_path = "index.css", // name of your styles file
		.html_path  = "index.html", // name of your html file
		.title      = "carol's page!"
	};
	
	/* now you  just create your page by initializing it with the `page_begin(page_conf*)` function. */
	page index = page_begin(&config);
	
	/* now you can just start building your page! */
	
		header("Hello, World!", &index);
		
		para("I am a paragraph and this little dude above me is a header!", &index);

	/* and, of course, don't forget to close your page when you're done */
	
	page_end(&index);
	
}

```

Now the only thing left to do is to initialize Carol:

```c

/* since Carol is a header-only library it must be implemented with the definition of CAROL_IMPLEMENTATION macro */
#define 	CAROL_IMPLEMENTATION
#include 	"carol.h"

int main(int argc, char**argv)
{
	carol_init();
	return 0;
}

```

So now, when running your application, you must already see the localhost address printed in your terminal -- remember to open your HTML file related to
the ./project/ folder, so: http://localhost:4242/project/[YOUR_HTML_FILE_NAME].hmtl
