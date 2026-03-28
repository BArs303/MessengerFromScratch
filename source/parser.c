#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//input must be of the format /{type}: {data}
//input must be null terminated
Message *parse(char input[]) {
  
  char writable_input[strlen(input)+1];
  strcpy(writable_input, input);

  enum message_types type = INVALID;
  char *data = NULL;
  char *data_copy;

  if(input[0] == '/' && strchr(&input[1], ':')) {
    char *type_str = strtok(&writable_input[1], ":");
    if(!strcmp(type_str, "send")) type = TRANSMISSION;
    else if(!strcmp(type_str, "register")) type = REGISTER;
    else if(!strcmp(type_str, "close")) type = CLOSE_CONNECTION;
    else if(!strcmp(type_str, "choose")) type = INVALID;
    else type = INVALID;

    data = strtok(NULL, ":");
    data_copy = malloc(strlen(data) + 1);
    if(!data_copy) {
      fprintf(stderr, "Failed to allocate memory for data. Exiting the program.\n");
      exit(1);
    }

    strcpy(data_copy, data);
  }

  Message *result = malloc(sizeof(Message) + strlen(data));
  
  if(!result) {
    fprintf(stderr, "Failed to allocate memory for output of parse. Exiting the program.\n");
    exit(1);
  }

  memcpy(result->data, data_copy, strlen(data_copy));
  result->length = data_copy ? strlen(data_copy) : 0;
  result->type = type;

  return result;
}

