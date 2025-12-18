
#ifndef _CAROL_H
#define _CAROL_H

#include <arpa/inet.h>

char*strrep(const char*orig_,const char*rep_,const char*with_);
const char *ftos(const char *filename_);

// int render_content(void);

int header(const char*body_);
int paragraph(const char*body_);

void render(void);

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

#define PROJECT_FOLDER "../../"//NOTE: this must be removed later.

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

//int render_content(void);
char *idx="";

static inline int concat(char *str_, const char*new_str_){
	size_t str_length=strlen(str_);
	size_t new_str_length=strlen(new_str_);
	size_t new_length=str_length+new_str_length+1;
	str_=(char*)malloc(new_length);
	if(str_==NULL)
		return -1;
	snprintf(str_,new_length,"%s%s",str_,new_str_);
	if(str_==NULL)
		return -1;
	return 0;
}

int header(const char*body_){
	char*tags="<h1></h1>";
	char*final_body="";
	size_t new_length=strlen(tags)+strlen(body_)+1;
	final_body=(char*)malloc(new_length);
	if(final_body==NULL)
		return -1;
	snprintf(final_body,new_length,"<h1>%s</h1>",body_);
	if(final_body==NULL)
		return -1;
	concat(idx,body_);
	return 0;
}
int paragraph(const char*body_){
	char*tags="<p></p>";
	char*final_body="";
	size_t new_length=strlen(tags)+strlen(body_)+1;
	final_body=(char*)malloc(strlen(tags)+strlen(body_)+1);
	if(final_body==NULL)
		return -1;
	snprintf(final_body,new_length,"<p>%s</p>",body_);
	if(final_body==NULL)
		return -1;
	concat(idx,body_);
	return 0;
}

void render(void){
	printf("hello?\n");
	handle_client();
	return;
}

#define BUFFER_SIZE 1024

int handle_client(void){//int skt_){
	char buffer[BUFFER_SIZE]={0};
//	if ((read(skt_,buffer,BUFFER_SIZE))==-1)
	if((read(skt,buffer,BUFFER_SIZE))==-1)
		goto NOT_FOUND;
	char path[BUFFER_SIZE];
	char method[8];
	
	sscanf(buffer,"%s %s",method,path);

	const char*page=strcmp(path,"/")==0?"index.html":path+1;
	char file_path[BUFFER_SIZE];
	if (strcmp(page,"favicon.ico")==0)
		return 0;
	snprintf(file_path,BUFFER_SIZE,PROJECT_FOLDER"%s",page);
//	if(!file_path)
	if((sizeof(file_path)/sizeof(file_path[0]))==0)
		goto NOT_FOUND;
// TODO: check if index is empty;
	const char*index_content=ftos(file_path);
	char *content=strrep(index_content,"[user_content]",idx);
	if(content==NULL)
		goto NOT_FOUND;
	const char*http_header="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//	write(skt_,http_header,strlen(http_header));
//	write(skt_,content,strlen(content));
	write(skt,http_header,strlen(http_header));
	write(skt,content,strlen(content));
	
	printf("client handled successfully.\n");
//	close(skt_);
	close(skt);
	return 0;
NOT_FOUND:
	printf("404 not found.\n");
	const char*not_found="HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
//	write(skt_,not_found,strlen(not_found));
//	close(skt_);
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
//	struct sockaddr_in address;
//	int server;
//	int socket;
	
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
//		handle_client(socket);
	}
	close(svr);
	return 0;
}

#endif//CAROL_IMPLEMENTATION

