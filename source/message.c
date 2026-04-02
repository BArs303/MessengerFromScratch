#include <stdlib.h>
#include <string.h>
#include "message.h"

struct message_metadata extract_metadata(Message *msg) {
  size_t data_size;
  struct message_metadata result;

  result.receiver = NULL;
  result.sender = NULL;
  if(msg->type == TRANSMISSION && msg->length > sizeof(Message)) {
    data_size = msg->length - sizeof(Message); 
    //result.receiver = msg->data;
  }

  return result;
}

StringView* create_string_view(char *s) {
  StringView *result;
  size_t length;

  length = strlen(s);
  result = malloc(sizeof(StringView) + length);
  if(result == NULL) {
    exit(EXIT_FAILURE);
  }

  result->length = length;
  memcpy(result->string, s, length);
  return result;
}

Message* create_message(size_t data_size) {
  Message *result;
  result = malloc(sizeof(Message) + data_size);
  if(result == NULL) {
    exit(EXIT_FAILURE);
  }
  result->length = data_size;
  return result;
}

Message* message_append() {
  return NULL;
}


