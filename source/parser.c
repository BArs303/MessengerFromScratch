#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//input must be of the format /{type}: {data}
//input must be null terminated
struct Message *parse(char input[]) {
  
  char writable_input[strlen(input)+1];
  strcpy(writable_input, input);

  enum Type type = INVALID;
  char *data = NULL;
  char *data_copy;

  if(input[0] == '/' && strchr(&input[1], ':')) {
    char *type_str = strtok(&writable_input[1], ":");
    if(!strcmp(type_str, "SEND_MSG")) type = SEND_MSG;
    else if(!strcmp(type_str, "REGISTER")) type = REGISTER;
    else if(!strcmp(type_str, "CLOSE_CONNECTION")) type = CLOSE_CONNECTION;
    else if(!strcmp(type_str, "CHOOSE_PARTNER")) type = CHOOSE_PARTNER;
    else type = INVALID;

    data = strtok(NULL, ":");
    data_copy = malloc(strlen(data) + 1);
    if(!data_copy) {
      fprintf(stderr, "Failed to allocate memory for data. Exiting the program.\n");
      exit(1);
    }

    strcpy(data_copy, data);
  }

  struct Message *result = malloc(sizeof(struct Message));
  
  if(!result) {
    fprintf(stderr, "Failed to allocate memory for output of parse. Exiting the program.\n");
    exit(1);
  }

  *result = (struct Message) {
    .data = data_copy,
    .type = type,
    .data_size = strlen(data_copy),
  };

  return result;
}

