#include "message.h"

Message* unwrap_message(Darray *container) {
  Message *result;
  result = container->array;
  result->length = container->size;
  return result;
}

MessageParameter* unwrap_message_parameter(Darray *container) {
  MessageParameter *result;
  result = container->array;
  result->length = container->size;
  return result;
}

void clear_msg_container(Darray *container) {
	container->size = sizeof(Message);
}

Darray* create_message_container() {
  Darray *result;
  result = darray_new(sizeof(Message), 1);
  result->size = sizeof(Message);
  return result;
}

MessageParameter* find_message_parameter(Message *msg, enum message_parameter_types type) {
	MessageParameter *result;
	size_t i = 0;
	while(true) {
		result = get_message_parameter(msg, i);
		if(result ==  NULL || result->type == type) {
			return result;
		}
		i++;
	}
	return NULL;
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

MessageParameter* add_message_parameter(Darray *container, void *value, size_t size) {
	MessageParameter tmp;
	MessageParameter *result;
	size_t offset;
	offset = container->size;
	darray_merge(container, &tmp, sizeof(MessageParameter));
	darray_merge(container, value, size);
	result = (MessageParameter*)((char*)(container->array) + offset);
	result->length = size + sizeof(tmp);
	return result;
}
