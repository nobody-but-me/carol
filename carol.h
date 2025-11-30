
#ifndef _CAROL_H
#define _CAROL_H

#include <arpa/inet.h>

const char *ftos(const char *filename_);

int render_content(void);

int handle_client(const int skt_);
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

const char *ftos(const char *filename_)
{
	size_t buffer_size = 0;
	char *buffer;
	
	FILE *file = NULL;
	file = fopen(filename_, "r");
	if (file == NULL) {
		fprintf(stderr, "failed to open file.\n");
		return NULL;
	}
	if (fseek(file, 0, SEEK_END) < 0) {
		printf("file is blank.\n");
		fclose(file);
		return NULL;
	}
	buffer_size = ftell(file);
	buffer = (char*)malloc(buffer_size + 1);
	if (buffer == NULL) {
		fprintf(stderr, "failed to allocate memory to buffer.\n");
		fclose(file);
		return NULL;
	}
	if (fseek(file, 0, SEEK_SET) < 0) {
		printf("file is blank.\n");
		fclose(file);
		return  NULL;
	}
	fread(buffer, 1, buffer_size, file);
	if (ferror(file)) {
		fprintf(stderr, "failed to transcript file content to buffer.\n");
		fclose(file);
		return NULL;
	}
	const char *final_buffer = buffer;
	fclose(file);
	return final_buffer;
}

int render_content(void);

#define BUFFER_SIZE 1024

int handle_client(int skt_)
{
	char buffer[BUFFER_SIZE] = {0};
	if ((read(skt_, buffer, BUFFER_SIZE)) == -1)
		goto NOT_FOUND;
	
	char path[BUFFER_SIZE];
	char method[8];
	
	sscanf(buffer, "%s %s", method, path);
	
	const char *page = strcmp(path, "/") == 0 ? "index.html" : path + 1;
	char file_path[BUFFER_SIZE];
	if (strcmp(page, "favicon.ico") == 0)
		return 0;
	snprintf(file_path, BUFFER_SIZE, PROJECT_FOLDER"%s", page);
	const char *content = ftos(file_path);
	
	if (!content)
		goto NOT_FOUND;
	const char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	write(skt_, http_header, strlen(http_header));
	write(skt_, content, strlen(content));
	
	printf("client handled successfully.\n");
	close(skt_);
	return 0;
NOT_FOUND:
	printf("404 not found.\n");
	const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
	write(skt_, not_found, strlen(not_found));
	close(skt_);
	return -1;
}

int init_server(struct sockaddr_in *address_, const int port_)
{
	int opt = 1;
	int server;
	
	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1) {
		fprintf(stderr, "failed to init server.\n");
		return -1;
	}
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "failed to set opt socket.\n");
		return -1;
	}
	address_->sin_addr.s_addr = INADDR_ANY;
	address_->sin_port = htons(port_);
	address_->sin_family = AF_INET;
	
	if (bind(server, (struct sockaddr*)address_, sizeof(*address_)) == -1) {
		fprintf(stderr, "failed at server-address binding.\n");
		close(server);
		return -1;
	}
	if (listen(server, 3) == -1) {
		fprintf(stderr, "failed to listen server.\n");
		close(server);
		return -1;
	}
	return server;
}

int init(const int port_)
{
	struct sockaddr_in address;
	int server;
	int socket;
	
	int address_length = sizeof(address);
	server = init_server(&address, port_);
	if (server == -1)
		return -1;
	printf("server is running at http://localhost:%d.\n", port_);
	while (1) {
		if ((socket = accept(server, (struct sockaddr*)&address, (socklen_t*)&address_length)) < 0) {
			fprintf(stderr, "connection have not been accepted.\n");
			continue;
		}
		handle_client(socket);
	}
	close(server);
	return 0;
}

#endif//CAROL_IMPLEMENTATION

