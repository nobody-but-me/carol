
#ifndef _CAROL_H
#define _CAROL_H

#include <arpa/inet.h>

char*strrep(const char*orig_,const char*rep_,const char*with_);
const char *ftos(const char *filename_);
int concat(char**str_,const char*new_str_);

int header(const char*body_);
int paragraph(const char*body_);
int image(const char*image_path_);
int hyperlink(const char*body_,const char*hyperlink_);

void ready(void);
void loop(void);

int handle_client(void);//const int skt_);
int init_server(struct sockaddr_in *address_, const int port_);

int init(const int port_);

#endif//_CAROL_H
#ifdef CAROL_IMPLEMENTATION

#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define PROJECT_FOLDER "../../site/"//NOTE: this must be removed later.

// i don't like the fact these variables are global.
struct sockaddr_in addr;
int svr; int skt;

char*strrep(const char*orig_,const char*rep_,const char*with_){
	int len_rep,len_with,len_front,count;
	char *result,*ins,*tmp;
	if(!orig_||!rep_)
		return NULL;
	len_rep=strlen(rep_);
	if(len_rep==0)
		return NULL;
	if(!with_)
		with_="";
	len_with=strlen(with_);
// NOTE: bad practice: throwing away const-ness.	
	ins=(char*)orig_;
	for(count=0;(tmp=strstr(ins,rep_));++count)
		ins=tmp+len_rep;
	tmp=result=malloc(strlen(orig_)+(len_with-len_rep)*count+1);
	if(!result)
		return NULL;
	while(count--){
		ins=strstr(orig_,rep_);
		len_front=ins-orig_;
		tmp=strncpy(tmp,orig_,len_front)+len_front;
		tmp=strcpy(tmp,with_)+len_with;
		orig_+=len_front+len_rep;
	}
	strcpy(tmp,orig_);
	return result;
}

const char*ftos(const char*filename_){
	size_t buffer_size=0;
	char *buffer;
	
	FILE*file=NULL;
	file=fopen(filename_,"r");
	if(file==NULL){
		fprintf(stderr,"failed to open file.\n");
		return NULL;
	}
	if(fseek(file,0,SEEK_END)<0){
		printf("file is blank.\n");
		fclose(file);
		return NULL;
	}
	buffer_size=ftell(file);
	buffer=(char*)malloc(buffer_size+1);
	if(buffer==NULL){
		fprintf(stderr,"failed to allocate memory to buffer.\n");
		fclose(file);
		return NULL;
	}
	if(fseek(file,0,SEEK_SET)<0){
		printf("file is blank.\n");
		fclose(file);
		return NULL;
	}
	fread(buffer,1,buffer_size,file);
	if(ferror(file)){
		fprintf(stderr,"failed to transcript file content to buffer.\n");
		fclose(file);
		return NULL;
	}
	const char*final_buffer=buffer;
	fclose(file);
	return final_buffer;
}

int concat(char**str_, const char*new_str_){
	size_t new_len=strlen(*str_)+strlen(new_str_)+1;
	char*old_str=(char*)malloc(strlen(*str_)+1);
	if(old_str==NULL)
		return -1;
	strcpy(old_str,*str_);
	*str_=(char*)malloc(new_len);
	if(*str_==NULL)
		return -1;
	snprintf(*str_,new_len,"%s%s",old_str,new_str_);
	if(*str_==NULL)
		return -1;
//	printf("str_: %s.\n",*str_);
	return 0;
}

// dont like its global
int initialized=0;
char last_tag[15];
char *idx="";

int header(const char*body_){
	if(initialized==1)
// perhaps you should not return one here :: it may lead to undefined behaviour
		return 1;
	char*tags="<h1>";char*body="";
	size_t last_tag_length=(sizeof(last_tag)/sizeof(last_tag[0]));
	
	size_t new_length=last_tag_length+strlen(tags)+strlen(body_)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
		return -1;
	if(last_tag_length>2)
		snprintf(body,new_length,"%s<h1>%s",last_tag,body_);
	else
		snprintf(body,new_length,"<h1>%s",body_);
	if(body==NULL)
		return -1;
	concat(&idx,body);
	strncpy(last_tag,"</h1>",strlen(tags)+2);
	return 0;
}
int paragraph(const char*body_){
	if(initialized==1)
		return 1;
	char*tags="<p>";char*body="";
	size_t last_tag_length=(sizeof(last_tag)/sizeof(last_tag[0]));
	
	size_t new_length=last_tag_length+strlen(tags)+strlen(body_)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
		return -1;
	if(last_tag_length>2)
		snprintf(body,new_length,"%s<p>%s",last_tag,body_);
	else
		snprintf(body,new_length,"<p>%s",body_);
	if(body==NULL)
		return -1;
	concat(&idx,body);
	strncpy(last_tag,"</p>",strlen(tags)+2);
	return 0;
}
int hyperlink(const char*body_,const char*hyperlink_){
	if(initialized==1)
		return 1;
	char*tags="<a href=''>";char*body="";
	size_t last_tag_length=(sizeof(last_tag)/sizeof(last_tag[0]));
	
	size_t new_length=last_tag_length+strlen(tags)+strlen(body_)+strlen(hyperlink_)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
		return -1;
	if(last_tag_length>2)
		snprintf(body,new_length,"%s<a href='%s'>%s",last_tag,hyperlink_,body_);
	else
		snprintf(body,new_length,"<a href='%s'>%s",hyperlink_,body_);
	if(body==NULL)
		return -1;
	concat(&idx,body);
	strncpy(last_tag,"</a>",strlen(tags)+2);
	return 0;
}

int image(const char*image_path_){
	if(initialized==1)
		return 1;
	char*tags="<img src=''/>";char*body="";
	size_t last_tag_length=(sizeof(last_tag)/sizeof(last_tag[0]));
	
	size_t new_length=last_tag_length+strlen(tags)+strlen(image_path_)+1;
	body=(char*)malloc(new_length);
	if(body==NULL)
		return -1;
	if(last_tag_length>2)
		snprintf(body,new_length,"%s<img src='%s'/>",last_tag,image_path_);
	else
		snprintf(body,new_length,"<img src='%s'/>",image_path_);
	if(body==NULL)
		return -1;
	concat(&idx,body);
	strncpy(last_tag," ",2);
	return 0;
}

#define BUFFER_SIZE 1024
#define STR_BUFFER_LENGTH 64

void ready(void){
	strncpy(last_tag," ",2);
	last_tag[(sizeof(last_tag)/sizeof(last_tag[0]))-1]='\0';
	idx="";
	return;
}

char *html_head="<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Razomentalist</title></head><body>";
char *html_footer="</body></html>";

int handle_client(void){
	char buffer[BUFFER_SIZE]={0};
	if((read(skt,buffer,BUFFER_SIZE))==-1)
		goto NOT_FOUND;
	char path[BUFFER_SIZE];
	char method[8];
	
	sscanf(buffer,"%s %s",method,path);
	
// hopefully it'll be useful in the future
	const char*page=strcmp(path,"/")==0?"index.html":path+1;
	char filepath[128];
	if (strcmp(page,"favicon.ico")==0)
		return 0;
	snprintf(filepath,128,PROJECT_FOLDER"%s",page);
	if((sizeof(filepath)/sizeof(filepath[0]))==0)
		goto NOT_FOUND;
	
	if(strlen(idx)<=1)
		return -1;
	printf("final HTML buffer:\n\n%s\n\n",idx);
	
	char *content=NULL;
	size_t length;
	FILE*f=fopen(filepath,"w");
	
	if(f){
		fputs(html_head,f);
			fputs(idx,f);
		fputs(html_footer,f);
		fclose(f);f=fopen(filepath,"rb");//do i really need to close and reopen the file after fputs()?
//  -- -- -- -- -- 
		fseek(f,0,SEEK_END);
		length=ftell(f);
		fseek(f,0,SEEK_SET);
		content=(char*)malloc(length+1);
		if(content)
			fread(content,1,length,f);
		printf("content:\n\n%s\n\n",content);
		fclose(f);
	}
	if(content){
		const char*http_header="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
		write(skt,http_header,strlen(http_header));
		write(skt,content,strlen(content));
		
		printf("client handled successfully.\n");
		close(skt);
	
		initialized=1;
	} else
		goto NOT_FOUND;
	free(content);
	return 0;
NOT_FOUND:
	printf("404 not found.\n");
	const char*not_found="HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
	write(skt,not_found,strlen(not_found));
	close(skt);
	return -1;
}

int init_server(struct sockaddr_in*address_, const int port_){
	int opt=1;
	int server;
	
	server=socket(AF_INET,SOCK_STREAM,0);
	if(server==-1) {
		fprintf(stderr,"failed to init server.\n");
		return -1;
	}
	if(setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))!=0){
		fprintf(stderr,"failed to set opt socket.\n");
		return -1;
	}
	address_->sin_addr.s_addr=INADDR_ANY;
	address_->sin_port=htons(port_);
	address_->sin_family=AF_INET;
	
	if(bind(server,(struct sockaddr*)address_,sizeof(*address_))==-1){
		fprintf(stderr,"failed at server-address binding.\n");
		close(server);
		return -1;
	}
	if(listen(server,3)==-1){
		fprintf(stderr,"failed to listen server.\n");
		close(server);
		return -1;
	}
	return server;
}

int init(const int port_){
	int address_length=sizeof(addr);
	svr=init_server(&addr,port_);
	if (svr==-1)
		return -1;
	printf("server is running at http://localhost:%d.\n",port_);
	while (1){
		if((skt=accept(svr,(struct sockaddr*)&addr,(socklen_t*)&address_length))<0) {
			fprintf(stderr,"connection have not been accepted.\n");
			continue;
		}
		ready();
		loop();
		handle_client();
	}
	close(svr);
	free(idx);
	return 0;
}

#endif//CAROL_IMPLEMENTATION

