
# Carol

Carol is a header-only static website generator written in C programming language. Following the philosophy of 
[j3s](https://j3s.sh/thought/my-website-is-one-binary.html), Carol generates HTML files based in one 
or more C files, running it in localhost -- all within a single-binary. Carol is meant to be simple, easy,
fast, fun and reliable.

## Generating a website

Carol mainly works with C functions:

```c

header("That's how you create a header!");

paragraph("and that's how you create a paragraph!");

```

But, before you can start writing your website in C, you need first to start Carol:

```c

// Since Carol is a header-only library, it must be implemented with the definition of CAROL_IMPLEMENTATION macro.
#define CAROL_IMPLEMENTATION
#include "carol.h"

int main(int argc, char**argv)
{
	init(8080);
	return 0;
}

```

And that's it! The functions used to build your website such as ```header(const char*)``` or ```paragraph(const char*)```
must be called within a function called ```loop```:

```c

void loop(void)
{
	header("Hello, World!");
	hyperlink("Click Me!", "https://github.com/nobody-but-me/carol");
}

```

So then, when running your application, you must already see the localhost address to be opened in your browser.
