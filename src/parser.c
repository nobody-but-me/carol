
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "./include/parser.h"


const char *file_to_char(const char *file_path)
{
    long buffer_size = 0;
    char *buffer;
    FILE *file;
    
    file = fopen(file_path, "r");
    if (file == NULL) {
	fprintf(stderr, "[FAILED] : File could not be loaded. \n");
	fclose(file);
	return NULL;
    }
    if (fseek(file, 0, SEEK_END) < 0) goto BLANK;
    buffer_size = ftell(file);
    buffer = (char *)malloc(buffer_size + 1);
    if (buffer == NULL) {
	fprintf(stderr, "[FAILED] : Buffer could not be read. \n");
	return NULL;
    }
    
    if (fseek(file, 0, SEEK_SET) < 0) goto BLANK;
    fread(buffer, 1, buffer_size, file);
    if (ferror(file)) {
	fprintf(stderr, "[FAILED] : File could not be read. \n");
	return NULL;
    }
    fclose(file);
    
    printf("[INFO] : File have been loaded sucessfully. \n");
    const char *result = buffer;
    return result;
    
BLANK:
    fprintf(stderr, "[INFO] : File is blank. \n");
    fclose(file);
    
    buffer = (char *)malloc(sizeof(char));
    free(buffer);
    return NULL;
}

// NOTE: This functions seems a little out of this script. Perhaps replace it would be a good idea.
char *load_file(const char *file_path)
{
    // Code stole from stackoverflow.
    FILE *file = fopen(file_path, "r");
    if (!file) {
	fprintf(stderr, "[FAILED] : File could not be recognized or does not exist.\n");
	return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *data = malloc(size + 1);
    fread(data, 1, size, file);
    data[size] =  '\0';
    fclose(file);
    return data;
}

static char *read_string(const char **text)
{
    const char *start = *text;
    while (**text != '[' && **text != '\0') {
	(*text)++;
    }
    size_t   length = (size_t)(*text - start);
    char *result = (char*)malloc(length + 1);
    strncpy(result, start, length);
    result[length] = '\0';
    return result;
}

char *read_header(const char **text)
{
    if (**text != '[') return NULL;
    (*text)++;
    
    if (strncmp(*text, "header]", 7) == 1)  return NULL;
    *text += 7;
    
    char *result = read_string(text);
    
    if (strncmp(*text, "[header]", 8) == 0) {
	(*text) += 8;
    };
    return result;
}

char *read_para(const char **text)
{
    if (**text != '[') return NULL;
    (*text)++;
    
    if (strncmp(*text, "para]", 5) == 1)  return NULL;
    *text += 5;
    
    char *result = read_string(text);
    
    if (strncmp(*text, "[para]", 6) == 0) {
	(*text) += 6;
    };
    return result;
}

void jump_space(const char ** text)
{
    while(**text == ' ' || **text == '\n') {
	(*text)++;
    }
    return;
}

RadownValue *parse(const char **text)
{
    jump_space(text);
    
    if (strncmp(*text, "[para]", 6) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	value->type = TYPE_PARA;
	value->para_value = read_para(text);
	return value;
    }
    else if (strncmp(*text, "[header]", 8) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	value->type = TYPE_HEADER;
	value->header_value = read_header(text);
	return value;
    }
    return NULL;
}

char *lexer(const char *tmp)
{
    RadownValue *main = parse(&tmp);
    char *content = "";
    while (main != NULL) {
	// TODO: Refactoring needed.
	char *old_content = content;
	char *new_content;
	
	if (main->type == TYPE_HEADER) {
	    char *tag = "<h1></h1>";
	    size_t length = sizeof(char) * (strlen(main->header_value) + strlen(tag) + 1);
	    new_content = (char*)malloc(length);
	    
	    sprintf(new_content, "<h1>%s</h1>", main->header_value);
	} else {
	    char *tag = "<p></p>";
	    size_t length = sizeof(char) * (strlen(main->header_value) + strlen(tag) + 1);
	    new_content = (char*)malloc(length);
	    
	    sprintf(new_content, "<p>%s</p>", main->header_value);
	}
	
	content = (char*)malloc((strlen(content) * 2) + strlen(new_content) + 1);
	sprintf(content, "%s%s", old_content, new_content);
	main = parse(&tmp);
    }
    
    return content;
}

