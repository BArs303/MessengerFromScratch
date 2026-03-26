#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include "message.h"

extern const char *const Type_Names[];

Message *parse(char input[]);

#endif
