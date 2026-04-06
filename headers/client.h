#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
typedef struct session_data SessionData;

struct session_data {
  Darray *message_container;
  Darray *partner_username;
};

void parse(char *user_input, SessionData current_session);

#endif
