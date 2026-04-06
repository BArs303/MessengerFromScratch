#include "message.h"

Message* get_message(Darray *message_container) {
  Message *result;
  result = message_container->array;
  result->length = message_container->size;
  return message_container->array;
}

Darray* create_message_container() {
  Darray *result;
  result = darray_new(sizeof(Message), 1);
  result->size = sizeof(Message);
  return result;
}

MessageParameter* get_message_parameter(Message *msg, size_t index) {
  MessageParameter *current_parameter;
  uint8_t *parray;
  size_t counter, offset, array_size;

  if(msg->length < sizeof(Message))
      return NULL;
  array_size = msg->length - sizeof(Message);
  for(counter = 0, offset = 0; counter <= index; counter++) {
    parray = msg->parameters;
    current_parameter = (MessageParameter*)(parray + offset);
    if(offset >= array_size) {
      return NULL;
    }
    offset += current_parameter->length;
  }
  return current_parameter;
}
