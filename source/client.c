#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "message.h"
#include "parser.h"

#define BUFFER_SIZE 1024

const char *const Type_Names[] = {"INVALID", "SEND_MSG", "CLOSE_CONNECTION", "REGISTER", "CHOOSE_PARTNER"};
int main() {
  
  // struct Message *msg_test = parse("/SEND_MSG: Hello");
  // printf("%s\n", msg_test->data);
  // printf("%s\n", Type_Names[msg_test->type]);
  // printf("%zu\n", msg_test->data_size);
  int sfd, opt;
  int p;
  struct sockaddr_in addr;
  socklen_t size;
  char buf[BUFFER_SIZE];
  ssize_t buf_size;
  Message *msg;

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


    buf_size = sizeof(Message) + strlen(buf);

    msg = malloc(buf_size);
    msg->type = USERNAME;
    msg->msg_length = buf_size;
    memcpy(msg->data, buf, buf_size - sizeof(Message));

    buf_size = write(sfd, msg, buf_size);
    if(buf_size == -1) {
      perror("error writing to the server\n");
      exit(EXIT_FAILURE);
    }
    sleep(2);
  }

  close(sfd);
  return 0;
}
