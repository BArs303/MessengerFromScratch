#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>
#include <stddef.h>

typedef struct message Message;

enum message_types {
  REGISTER,
  TRANSMISSION,
  IP,
  INVALID,
  CLOSE_CONNECTION,
};

struct message {
  enum message_types type:16;
  uint16_t length;
  char data[];
};

/* 
 * A helper structure that contains pointers to the metadata 
 * in the received message
 */
struct message_metadata {
  char *receiver;
  char *sender;
};

struct network_string {
  size_t length;
  char string[];
};

struct message_metadata extract_metadata(Message *msg);

#endif
