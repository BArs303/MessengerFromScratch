#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct message Message;

enum message_types {
  USERNAME,
  TRANSMISSION,
};

struct message {
  enum message_types type;
  unsigned int msg_length;
  char data[];
};

#endif
