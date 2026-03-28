#include "message.h"
#include <string.h>

struct message_metadata extract_metadata(Message *msg) {
  size_t data_size;
  struct message_metadata result;

  result.receiver = NULL;
  result.sender = NULL;
  if(msg->type == TRANSMISSION && msg->length > sizeof(Message)) {
    data_size = msg->length - sizeof(Message); 
    /*
     * possible error if msg->data is not null terminated
     */
    result.receiver = msg->data;
  }

  return result;
}
