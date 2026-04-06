#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>
#include <stddef.h>
#include "dynamic_array.h"

typedef struct message Message;
typedef struct string_view StringView;
typedef struct message_parameter MessageParameter;

enum message_parameter_types {
    RECEIVER_USERNAME,
    SENDER_USERNAME,
    TEXT,
};

enum message_types {
  REGISTER,
  TRANSMISSION,
  CHOOSE,
  IP,
  INVALID,
  CLOSE_CONNECTION,
};

struct message {
  enum message_types type:16;
  uint16_t length;
  uint8_t parameters[];
};

struct message_parameter {
  enum message_parameter_types type:16;
  uint16_t length;
  uint8_t value[];
};

struct string_view {
  size_t length;
  char string[];
};

Darray* create_message_container();
Message* get_message(Darray *message_container);
MessageParameter* get_message_parameter(Message *msg, size_t index);

#endif
