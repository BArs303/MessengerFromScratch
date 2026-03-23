#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>

typedef struct message Message;

enum message_types {
  USERNAME,
  TRANSMISSION,
};

struct message {
  enum message_types type:16;
  uint16_t length;
  char data[];
};

#endif
