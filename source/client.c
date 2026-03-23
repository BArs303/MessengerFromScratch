#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

enum Type {
  SEND_MSG,
  CLOSE_CONNECTION,
  REGISTER,
  CHOOSE_PARTNER,
};

struct Message {
  enum Type type;
  char *data;
  size_t data_size;
};

//parse user input of the form /type: {data} and creates the struct Message out of it.
//Assumes user_input is null terminated.
struct Message *parse_user_input(const char *user_input, size_t len) {
  char type_str[100] = "\0";
  enum Type type;
  char *data = malloc(len + 1);
  
  if(!data) {
    perror("Failed to allocate memory.\n");
    exit(1);
  }

  strcpy(data,"\0");
  enum Cursor_Position {
    BEFORE_SLASH,
    BTW_SLASH_COLON,
    AFTER_COLON
  };
  
  enum Cursor_Position cp = BEFORE_SLASH;

  size_t j = 0;
  size_t i;
  for(i = 0; i < len; i++) {
    
    if(cp == BEFORE_SLASH) {
      
      if(isspace(user_input[i])) continue;
      if(user_input[i] == '/') {
        cp = BTW_SLASH_COLON; 
        continue;
      }
      
      strcpy(type_str, "SEND_MSG");
      strcpy(data, &user_input[i]);
      break;
    
    } else if (cp == BTW_SLASH_COLON) {
      
      if(j > 98) {
        fprintf(stderr, "Command type greater than allowed size. Subsequent command input ignored.\n");
        cp = AFTER_COLON; 
        continue;
      }


      if(user_input[i] == ':') {
        cp = AFTER_COLON;
        continue;
      }

      type_str[j++] = user_input[i];
    
    } else if (cp == AFTER_COLON) {
      strcpy(data, &user_input[i]);
      break;
    }
  }

  type_str[j] = 0;

  if(!strcmp(type_str, "SEND_MSG")) type = SEND_MSG;
  else if(!strcmp(type_str, "CLOSE_CONNECTION")) type = CLOSE_CONNECTION;
  else if(!strcmp(type_str, "REGISTER")) type = REGISTER;
  else if(!strcmp(type_str, "CHOOSE_PARTNER")) type = CHOOSE_PARTNER;
  else type = SEND_MSG;

  struct Message *result = malloc(sizeof(struct Message));
  if(!result) {
    perror("Failed to allocate memory\n");
    exit(1);
  }
  *result = (struct Message){.data = data, .data_size = len - i, .type = type};

  return result;
}


int main() {
  int sfd, opt;
  int p;
  struct sockaddr_in addr;
  socklen_t size;
  char buf[BUFFER_SIZE];
  ssize_t buf_size;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sfd == -1) {
    perror("create socket error\n");
    exit(EXIT_FAILURE);
  }
  opt = 1;
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  addr.sin_family = AF_INET;
  /* local address 127.0.0.1 */
  addr.sin_addr.s_addr = htonl(0x7F000001);
  /* server port */
  addr.sin_port = htons(9997);

  size = sizeof(addr);

  if(connect(sfd, (struct sockaddr*) &addr, size) == -1) {
    perror("connection error\n");
    exit(EXIT_FAILURE);
  }
  p = fork();
  if(p == -1) {
    perror("cannot create a process\n");
  }
  else if(p == 0) {
    /* child process */
    buf_size = read(sfd, buf, BUFFER_SIZE);
    if(buf_size == -1) {
      perror("error reading from the server\n");
      exit(EXIT_FAILURE);
    }
    buf[buf_size] = 0;
    printf("Server response: %s\n", buf);
  }
  else {
    /* parent process */
    printf("Enter your username\n");
    if(fgets(buf, BUFFER_SIZE, stdin) == NULL) {
      perror("read from stdin error\n");
      exit(EXIT_FAILURE);
    }

    buf_size = write(sfd, buf, BUFFER_SIZE);
    if(buf_size == -1) {
      perror("error writing to the server\n");
      exit(EXIT_FAILURE);
    }
  }
  
  close(sfd);
  return 0;
}
