#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "client.h"

#define BUFFER_SIZE 1024

int establish_connection(int address, int port) {
  int socket_fd, opt;
  struct sockaddr_in addr;
  socklen_t addr_size;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1) {
    perror("create socket error\n");
    exit(EXIT_FAILURE);
  }
  opt = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(address);
  addr.sin_port = htons(port);

  addr_size = sizeof(addr);
  if(connect(socket_fd, (struct sockaddr*) &addr, addr_size) == -1) {
    perror("connection error\n");
    exit(EXIT_FAILURE);
  }
  return socket_fd;

}

void init_session(SessionData *current_session) {

  current_session->message_container = create_message_container();
  current_session->partner_username = darray_new(DARRAY_INITIAL_CAPACITY, 1);

  /*current_session->local_username = darray_new(sizeof(MessageParameter), 1);*/
}

int main() {

  int sfd, pid;
  size_t buf_size;
  char buf[BUFFER_SIZE];
  struct session_data current_session;
  Message *msg;

  init_session(&current_session);
  sfd = establish_connection(0x7F000001, 9997);

  pid = fork();
  if(pid == -1) {
    perror("cannot create a process\n");
  }
  else if(pid == 0) {
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
    while(1) {
      if(fgets(buf, BUFFER_SIZE, stdin) == NULL) {
        perror("read from stdin error\n");
        exit(EXIT_FAILURE);
      }
	  msg = form_message(buf, current_session);
	  printf("Message size: %hu\nMessage type: %d\n", msg->length, msg->type);
      buf_size = write(sfd, msg, msg->length);
      if(buf_size == -1) {
        perror("error writing to the server\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  close(sfd);
  return 0;
}

Message* form_message(char *user_input, SessionData current_session) {
	char *command, *data;
	Darray *container;
	Message *current_message;
	MessageParameter *parameter;
	enum message_types type;

	command = strtok(user_input, " ");
	data = user_input + strlen(command) + 1;
	container = current_session.message_container;

	clear_msg_container(current_session.message_container);
	if(strcmp(command, "/send") == 0) {
		type = TRANSMISSION;
		/*parameter = add_message_parameter(container, data, strlen(data));
		parameter->type = CHOOSE;*/

	}
	else if(strcmp(command, "/choose") == 0) {
		/* check function result */
		current_session.partner_username->size = 0;
		darray_merge(current_session.partner_username, data, strlen(data));
		type = CHOOSE;
	}
	else if(strcmp(command, "/register") == 0) {
		parameter = add_message_parameter(container, data, strlen(data) + 1);
		parameter->type = SENDER_USERNAME;
		type = REGISTER;
	}
	else if(strcmp(command, "/exit") == 0) {
		type = CLOSE_CONNECTION;
	}
	else {
		type = INVALID;
		printf("Unknown command %s\n", command);
	}
	/* check function result */
	current_message = unwrap_message(current_session.message_container);
	current_message->type = type;
	return current_message;
}
