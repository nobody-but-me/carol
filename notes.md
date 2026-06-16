
## problem with char*buffer

currently char*buffer is concatenated many times with malloc, memcpy and free.
It would significantly better if it was upgrade to be reallocated in lots,
with a certain capacity in a string-builder model.

## bigger pages problem

currently the final HTML file is written by page_end, the entire thing --
through char*buffer. It is going to be a problem for bigger pages because
it's going to take so long for them to be written. A solution would be
write directly to the HTML file when element functions (header, image, para,
et cetera) are called.

## better element struct

```

typedef struct element
{
    char *html;
    struct element *next;
} element;

...

typedef struct page
{
	...
	element *first;
	element *last;
	...
}

...

fputs(first->html); //

```

simply better.


## carol_compose without serving

title. 

