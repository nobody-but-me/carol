
#ifndef CAROL_H
#define CAROL_H

int header(const char*body_);
int paragraph(const char*body_);
int hyperlink(const char*body_,const char*hyperlink_);
int image(const char*image_path_);

void render(void);
int init(void);

#endif//CAROL_H
#ifdef CAROL_IMPLEMENTATION

#include <ctype.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#include <errno.h>

#define DEFAULT_BUFFER_SIZE 8192
#define DEFAULT_PORT 4242

static int concat(char**str_, const char*new_str_){
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
	return 0;
}

int initialized=0;
char last_tag[15];
char*idx="";

int header(const char*body_)
{
	if(initialized==1)
		return 1;
	char*tags="<h1>";char*body="";
	size_t last_tag_length=(sizeof(last_tag)/sizeof(last_tag[0]));
	
	size_t  new_length=last_tag_length+strlen(tags)+strlen(body_)+1;
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
	printf("header element generated.\n");
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
	printf("paragraph element generated.\n");
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
	printf("hyperlink element generated.\n");
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
	printf("image element generated.\n");
	return 0;
}

static const char*get_file_extension(const char*file_name)
{
	const  char*dot=strrchr(file_name,'.');
	if(!dot||dot==file_name)
		return "";
	return dot+1;
}

static const char *get_filetype(const char*file_extension_)
{
	if(strcasecmp(file_extension_,"html")==0||strcasecmp(file_extension_,"htm")==0)
		return "text/html";
	else if(strcasecmp(file_extension_,"txt")==0)
		return "text/plain";
	else if(strcasecmp(file_extension_,"jpg")==0||strcasecmp(file_extension_,"jpeg")==0)
		return "image/jpeg";
	else if(strcasecmp(file_extension_,"png")==0)
		return "image/png";
	else if(strcasecmp(file_extension_,"webp")==0)
		return "image/webp";
	else
		return "application/octet-stream";
}

static char*url_decode(const char*src_)
{
	size_t src_length=strlen(src_);
	char*decoded=malloc(src_length+1);
	size_t decoded_length=0;
	
	for(size_t i=0;i<src_length;i++){
		if(src_[i]=='%'&&i+2<src_length){
			int value;
			sscanf(src_+i+1,"%2x",&value);
			decoded[decoded_length++]=value;
			i+=2;
		} else {
			decoded[decoded_length++]=src_[i];
		}
	}
	decoded[decoded_length]='\0';
	return decoded;
}

char *html_head="<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Razomentalist</title></head><body>";
char *html_footer="</body></html>";

static void*handle_client(void*client_)
{
	int client=*((int*)client_);
	char*buffer=(char*)malloc(DEFAULT_BUFFER_SIZE*sizeof(char));
	
	ssize_t received=recv(client,buffer,DEFAULT_BUFFER_SIZE,0);
	if(received>0){
		regex_t reg;
		regcomp(&reg,"^GET /([^ ]*) HTTP/1",REG_EXTENDED);
		regmatch_t matches[2];
		
		if(regexec(&reg,buffer,2,matches,0)==0){
			buffer[matches[1].rm_eo]='\0';
			const char*encoded=buffer+matches[1].rm_so;
			char*file_name=url_decode(encoded);
			char file_extension[32];
			strncpy(file_extension,get_file_extension(file_name),32);
			
#define PROJECT_FOLDER "../../site/"
// NOTE: I am adding this conditional mainly because in this
// way it'll be possible not only to serve and open html files, but 
// also other files types, such as png, jpg and webp.
//			printf("\n%s\n\n",file_extension);
			if(strcasecmp(file_extension,"html")==0||strcasecmp(file_extension,"htm")==0){
				if(strlen(idx)<=1){
					fprintf(stderr,"index is empty or NULL\n");
					return NULL;
				}
// NOTE: for now it'll only support the index main html file.
				const char*html_index="index.html";
				FILE*f=fopen(html_index,"w");
				if(f){
					printf("writing to html file...\n");
					fputs(html_head,f);
						fputs(idx,f);
					fputs(html_footer,f);
					fclose(f);
				}
				initialized=1;
			}
			char*response=(char*)malloc(DEFAULT_BUFFER_SIZE*2*sizeof(char));
			
			const char*mime_type=get_filetype(file_extension);
			char*header=(char*)malloc(DEFAULT_BUFFER_SIZE*sizeof(char));
			snprintf(header,DEFAULT_BUFFER_SIZE,
				"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",mime_type);
			send(client,header,strlen(header),0);
			int file=open(file_name,O_RDONLY);
			if(file==-1){
				snprintf(response,DEFAULT_BUFFER_SIZE,"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>");
				return NULL;
			}
			memcpy(response,header,strlen(header));
			
			ssize_t bytes_read;
			while((bytes_read=read(file,buffer,sizeof(buffer)))>0){
				ssize_t total=0;
				while(total<bytes_read){
					ssize_t sent=send(client,buffer+total,bytes_read-total,0);
					if(sent<=0)
						return NULL; // TODO: review this piece of shit later.
					total+=sent;
				}
			}
// TODO: review this too.
			free(file_name);
			free(response);
			free(header);
			close(file);
		}
		regfree(&reg);
	}
	close(client);
	free(client_);
	free(buffer);
	return NULL;
}

static int init_server(int*server_,int*opt_,struct sockaddr_in*server_address_)
{
	if((*server_=socket(AF_INET,SOCK_STREAM,0))<0){
		fprintf(stderr,"socket failed\n");
		return -1;
	}
	if(setsockopt(*server_,SOL_SOCKET,SO_REUSEADDR,&opt_,sizeof(*opt_))<0){
		fprintf(stderr,"socket opt setting failed\n");
		return -1;
	}
	server_address_->sin_family=AF_INET;
	server_address_->sin_addr.s_addr=INADDR_ANY;
	server_address_->sin_port=htons(DEFAULT_PORT);
	
	if(bind(*server_,(struct sockaddr*)server_address_,sizeof(*server_address_))<0){
		fprintf(stderr,"binding failed\n");
		return -1;
	}
	if(listen(*server_,10)<0){
		fprintf(stderr,"listening failed\n");
		return -1;
	}
	printf("listening at http://localhost:%d/index.html\n\n",DEFAULT_PORT);
	return 0;
}

int init(void)
{
	int server; int opt=1;
	struct sockaddr_in server_addr;
	if(init_server(&server,&opt,&server_addr)!=0)
		return -1;
	while(1){
		struct sockaddr_in client_address;
		socklen_t client_address_length=sizeof(client_address);
		int *client=malloc(sizeof(int));
		if((*client=accept(server,(struct sockaddr*)&client_address,&client_address_length))<0){
			fprintf(stderr,"acception failed");
			continue;
		}
		render();
		handle_client((void*)client);
	}
	close(server);
	return 0;
}

#endif//CAROL_IMPLEMENTATION
