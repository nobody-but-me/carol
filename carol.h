
#ifndef CAROL_H
#define CAROL_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    const char*style_path;
    const char*html_path;
    
    const char*title;
    
    bool initialized;
} page_conf;

typedef struct
{
    page_conf*configuration;
    char last_tag[16];
    
    FILE*output;
    char*buffer; // TODO: change its name to buffer
} page;

void hyperlink(const char*_link,const char*_body,page*_page);
void header(const char*_body,page*_page);
void para(const char*_body,page*_page);

page page_begin(page_conf*_configuration);
void page_end(page*_page);

void carol_render(void);
int carol_init(void);

#endif//CAROL_H
#ifdef CAROL_IMPLEMENTATION

#define DEFAULT_BUFFER_SIZE 8192
#define DEFAULT_PORT 4242

#include <dirent.h>
#include <ctype.h>

#include <arpa/inet.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <regex.h>
#include <errno.h>

static int concat(char**_str,const char*_new_str);

void header(const char*_body,page*_page)
{
    char*tag="<h1>";char*body;
    size_t last_tag_length=(sizeof(_page->last_tag)/sizeof(_page->last_tag[0]));
    
    if(_page->last_tag[0]!='\0')
    {
	size_t new_length=strlen(tag)+strlen(_body)+last_tag_length+2;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"%s<h1>%s",_page->last_tag,_body);
    } else
    {
	size_t new_length=strlen(tag)+strlen(_body)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"<h1>%s",_body);
    }
    concat(&_page->buffer,body);
    strncpy(_page->last_tag,"</h1>",strlen(tag)+2);
    
    printf("header element generated.\n");
    return;
}
void para(const char*_body,page*_page)
{
    char*tag="<p>";char*body;
    size_t last_tag_length=(sizeof(_page->last_tag)/sizeof(_page->last_tag[0]));
    
    if(_page->last_tag[0]!='\0')
    {
	size_t new_length=strlen(tag)+strlen(_body)+last_tag_length+2;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"%s<p>%s",_page->last_tag,_body);
    } else
    {
	size_t new_length=strlen(tag)+strlen(_body)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"<p>%s",_body);
    }
    if(body==NULL)
    return;
    concat(&_page->buffer,body);
    strncpy(_page->last_tag,"</p>",strlen(tag)+2);
    
    printf("paragraph element generated.\n");
    return;
}
void hyperlink(const char*_link,const char*_body,page*_page)
{
    char*tag="<a href=''>";char*body;
    size_t last_tag_length=(sizeof(_page->last_tag)/sizeof(_page->last_tag[0]));
    
    if(_page->last_tag[0]!='\0')
    {
	size_t new_length=strlen(tag)+strlen(_link)+strlen(_body)+last_tag_length+2;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"%s<a href='%s'>%s",_page->last_tag,_link,_body);
    } else
    {
	size_t new_length=strlen(tag)+strlen(_link)+strlen(_body)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
	return;
	snprintf(body,new_length,"<a href='%s'>%s",_link,_body);
    }
    if(body==NULL)
    return;
    concat(&_page->buffer,body);
    strncpy(_page->last_tag,"</a>",strlen(tag)+2);
    printf("hyperlink element generated.\n");
    return;
}

static int concat(char**_str,const char*_new_str)
{
    if(*_str==NULL)
    {
	printf("allocating more memory to _str...\n");
	size_t new_length=strlen(_new_str)+1;
	*_str=(char*)malloc(new_length);	
	if(*_str==NULL)
	{
	    fprintf(stderr,"failed to allocate memory to _str :: concat \n");
	    return -1;
	}
    }
    if(_new_str==NULL)
    {
	fprintf(stderr,"_new_str is NULL.\n");
	return -1;
    }
    size_t new_length=strlen(*_str)+strlen(_new_str)+1;
    char*old_str=(char*)malloc(strlen(*_str)+1);
    if(old_str==NULL)
    return -1;
    strcpy(old_str,*_str);
    *_str=(char*)malloc(new_length);
    if(*_str==NULL)
    return -1;
    snprintf(*_str,new_length,"%s%s",old_str,_new_str);
    if(*_str==NULL)
    return -1;
    return 0;
}

static const char*get_file_extension(const char*_filename)
{
    const char*dot=strrchr(_filename,'.');
    if(!dot||dot==_filename)
    return "";
    return dot+1;
}

static const char*get_filetype(const char*_file_extension)
{
    if(strcasecmp(_file_extension,"html")==0||strcasecmp(_file_extension,"htm")==0)
    return "text/html";
    else if(strcasecmp(_file_extension,"txt")==0)
    return "text/plain";
    else if(strcasecmp(_file_extension,"jpg")==0||strcasecmp(_file_extension,"jpeg")==0)
    return "text/jpeg";
    else if(strcasecmp(_file_extension,"png")==0)
    return "text/png";
    else if(strcasecmp(_file_extension,"webp")==0)
    return "image/webp";
    else if(strcasecmp(_file_extension,"css")==0)
    return "text/css";
    else
    return "application/octet-stream";
}

static char*url_decode(const char*_src)
{
    size_t src_length=strlen(_src);
    char*decoded=malloc(src_length+1);
    size_t decoded_length=0;
    
    for(size_t i=0;i<src_length;i++)
    {
	if(_src[i]=='%'&&i+2<src_length)
	{
	    int value;
	    sscanf(_src+i+1,"%2x",&value);
	    decoded[decoded_length++]=value;
	    i+=2;
	}
	else
	{
	    decoded[decoded_length++]=_src[i];
	}
    }
    decoded[decoded_length]='\0';
    return decoded;
}

char *g_html_header="<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>%s</title><link rel='stylesheet' type='text/css' href='%s'/></head><body>";
char *g_html_footer="</body></html>";

// const char*css_path="project/index.css";
page page_begin(page_conf*_configuration)
{
    const char*prefix="project/";
    char*final_header;
    page pg;
    
 // TODO: check for empty strings here;
    pg.configuration=_configuration;
    
    printf("initializing %s page...\n",pg.configuration->html_path);
    if(pg.configuration->style_path!=NULL)
    {
	size_t style_length=strlen(prefix)+strlen(pg.configuration->style_path)+1;
	char*final_style=(char*)malloc(style_length);
	if(!final_style)
	{
	    fprintf(stderr,"failed to allocate memory for style buffer.\n");
	    pg.output=NULL;
	    return pg;
	}
	snprintf(final_style,style_length,"%s%s",prefix,pg.configuration->style_path);
	FILE*st=fopen(final_style,"w");
	if(!st)
	{
	    fprintf(stderr,"failed to create or write to styles.css file.\n");
	    pg.output=NULL;
	    return pg;
	}
	fputs("						\n",st);
	fputs("body,html {					\n",st);
	fputs("	background-color: 	#FFFDD0;	\n",st);
	fputs("	padding: 		0;		\n",st);
	fputs("	margin:  		5px;		\n",st);
	fputs("	left:	 		0;		\n",st);
	fputs("	top:	 		0;		\n",st);
	fputs("						\n",st);
	fputs("	min-height:		100vh;		\n",st);
	fputs("	min-width:		100vw;		\n",st);
	fputs("						\n",st);
	fputs("	overflow:		hidden;	\n",st);
	fputs("}						\n",st);
	fputs("						\n",st);
	fclose(st);
    }
// building the final HTML header string
    size_t header_length=strlen(g_html_header)+strlen(pg.configuration->title)+strlen("./index.css")+1;
    final_header=(char*)malloc(header_length);
    if(final_header==NULL)
    {
	fprintf(stderr,"failed to allocate memory for final HTML header buffer.\n");
	pg.output=NULL;
	return pg;
    }
    snprintf(final_header,header_length,g_html_header,pg.configuration->title,"./index.css"); // TODO: hardcoded
    printf("\nfinal html header: %s\n\n",final_header);
    
// allocating memory to add prefix to page path
    size_t final_path_length=strlen(prefix)+strlen(pg.configuration->html_path)+1;
    char*final_path=(char*)malloc(final_path_length);
    if(final_path==NULL)
    {
	fprintf(stderr,"failed to allocate memory for final page buffer.\n");
	pg.output=NULL;
	return pg;
    }
    snprintf(final_path,final_path_length,"%s%s",prefix,pg.configuration->html_path);
    printf("\nfinal path: %s\n\n",final_path);
    
// writing to the actual HTML file
    pg.output=fopen(final_path,"w");
    if(pg.output==NULL)
    {
	fprintf(stderr,"failed to write to %s.\n",pg.configuration->html_path);
	return pg;
    }
// adding HTML header in it
    fputs(final_header,pg.output);
    
    pg.last_tag[0]='\0';
    pg.buffer=NULL;
    
    pg.configuration->initialized=true;
    free(final_header);
    free(final_path);
    
    printf("%s page initialized successfully.\n",pg.configuration->html_path);
    return pg;
}

void page_end(page*_page)
{
    if(_page==NULL)
        return;
    if(_page->output==NULL)
    {
	fprintf(stderr,"can't close  page: page output is NULL.\n");
	fclose(_page->output);
	return;
    }
    if(_page->buffer==NULL)
    {
	fprintf(stderr,"page's  index is NULL.\n");
	fclose(_page->output);
	return;
    }
    fputs(_page->buffer,_page->output);
    fputs(g_html_footer,_page->output);
    fclose(_page->output);
    
    printf("page closed successfully.\n");
    return;
}

static bool handle_client(int**_client)
{
    char*buffer=(char*)malloc(DEFAULT_BUFFER_SIZE*sizeof(char));
    
    ssize_t received=recv(**_client,buffer,DEFAULT_BUFFER_SIZE,0);
    if(received>0)
    {
	regex_t reg;
	regcomp(&reg,"^GET /([^ ]*) HTTP/1",REG_EXTENDED);
	regmatch_t matches[2];
	
	if(regexec(&reg,buffer,2,matches,0)==0)
	{
	    buffer[matches[1].rm_eo]='\0';
	    const char*encoded=buffer+matches[1].rm_so;
	    char*filename=url_decode(encoded);
	    char file_extension[32];
	    strncpy(file_extension,get_file_extension(filename),32);

	    const char*type=get_filetype(file_extension);
	    
	    if(strcasecmp(type,"text/html")==0)
	    carol_render();
	    
	    char*response=(char*)malloc(DEFAULT_BUFFER_SIZE*2*sizeof(char));
	    char*header=(char*)malloc(DEFAULT_BUFFER_SIZE*sizeof(char));
	    snprintf(header,DEFAULT_BUFFER_SIZE,
	    "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",type);
	    send(**_client,header,strlen(header),0);
	    
	    int file=open(filename,O_RDONLY);
	    if(file==-1)
	    {
		snprintf(response,DEFAULT_BUFFER_SIZE,
		"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>");
		return false;
	    }
	    memcpy(response,header,strlen(header));
	    
	    ssize_t bytes_read;
	    while((bytes_read=read(file,buffer,sizeof(buffer)))>0)
	    {
		ssize_t total=0;
		while(total<bytes_read)
		{
		    ssize_t sent=send(**_client,buffer+total,bytes_read-total,0);
		    if(sent<=0)
		    return false;
		    total+=sent;
		}
	    }
	    free(filename);
	    free(response);
	    free(header);
	    close(file);
	}
	regfree(&reg);
    }
    shutdown(**_client, SHUT_WR);
    close(**_client);
    free(buffer);
    
    printf("client handled successfully.\n");
    return true;
}

static int init_server(int*_server,int*_opt,struct sockaddr_in*_server_address)
{
    if((*_server=socket(AF_INET,SOCK_STREAM,0))<0)
    {
	fprintf(stderr,"socket failed.\n");
	return -1;
    }
    if(setsockopt(*_server,SOL_SOCKET,SO_REUSEADDR,&_opt,sizeof(*_opt))<0)
    {
	fprintf(stderr,"socket opt setting failed.\n");
	return -1;
    }
    _server_address->sin_family=AF_INET;
    _server_address->sin_addr.s_addr=INADDR_ANY;
    _server_address->sin_port=htons(DEFAULT_PORT);
    
    if(bind(*_server,(struct sockaddr*)_server_address,sizeof(*_server_address))<0)
    {
	fprintf(stderr,"binding failed.\n");
	return -1;
    }
    if(listen(*_server,10)<0)
    {
	fprintf(stderr,"listening failed.\n");
	return -1;
    }
    printf("listening  at http://localhost:%d.\n\n",DEFAULT_PORT);
    return 0;
}

int carol_init(void)
{
    mkdir("project",0777);
    
    int server; int opt=1;
    struct sockaddr_in server_addr;
    if(init_server(&server,&opt,&server_addr)!=0)
    return -1;
    while(true)
    {
	struct sockaddr_in client_address;
	socklen_t client_address_length=sizeof(client_address);
	int*client=malloc(sizeof(int));
	
	if((*client=accept(server,(struct sockaddr*)&client_address,&client_address_length))<0)
	{
	    fprintf(stderr,"acception failed");
	    continue;
	}
	handle_client(&client);
    }
    close(server);
    return 0;
}

#endif//CAROL_IMPLEMENTATION
