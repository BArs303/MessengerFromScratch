#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

enum Type {
  INVALID,
  SEND_MSG,
  CLOSE_CONNECTION,
  REGISTER,
  CHOOSE_PARTNER,
};

extern const char *const Type_Names[];

struct Message {
  enum Type type;
  char* data;
  size_t data_size;
};

struct Message *parse(char input[]);

#endif
