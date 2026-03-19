#include <stdlib.h>
#include <unistd.h>

/* socket, bind, listen */
#include <sys/socket.h>

#include <sys/epoll.h>

/* sockaddr_in */
#include <netinet/in.h>

/* for printf */
#include <stdio.h>

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "dynamic_array.h"

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

typedef enum client_states ClientStates;
typedef struct client Client;

enum client_states {
  WAITING_USERNAME,
};

struct _server {
  int listen_socket;
  int epoll_socket;
  Darray *clients;
};

struct client {
  int fd; 
  char *username;
  ClientStates state;
};


int create_listening_socket(unsigned short port);
void accept_conn_req(struct _server server);
void set_nonblocking(int ifd);
void process_event(struct _server server, struct epoll_event event);
Client create_client(int fd);
struct _server server_init(unsigned short port);
size_t find_fd_index(Darray *a, int fd);

struct _server server_init(unsigned short port) 
{
  struct _server result;
  int epoll_socket, listen_socket;
  struct epoll_event ev;

  listen_socket = create_listening_socket(port);
  epoll_socket = epoll_create1(0);
  if(epoll_socket == -1) {
    perror("epoll create error\n");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = listen_socket;
  if(epoll_ctl(epoll_socket, EPOLL_CTL_ADD, listen_socket, &ev) == -1) {
    perror("can't add listen socket to epoll\n");
    exit(EXIT_FAILURE);
  }

  result.clients = darray_new(0, sizeof(Client));
  result.listen_socket = listen_socket;
  result.epoll_socket = epoll_socket;
  return result;
}

Client create_client(int fd)
{
  Client result;
  result.fd = fd;
  result.state = WAITING_USERNAME;
  return result;
}

int main() 
{
  struct _server server;
  int nfds;
  struct epoll_event events[MAX_EVENTS];

  Client *tmp;

  server = server_init(9997);
  printf("listen socket %d\n", server.listen_socket);

  for(;;) {
    nfds = epoll_wait(server.epoll_socket, events, MAX_EVENTS, -1);
    if(nfds == -1) {
      /* epoll_wait error */
      exit(EXIT_FAILURE);
    }

    printf("List of client file descriptors: ");
    for(size_t i = 0; i < server.clients->size; i++) {
      tmp = darray_get(server.clients, i);
      printf("%d, ", tmp->fd);
    }
    printf("\n");
    for(int n = 0; n < nfds; n++) {
      if(events[n].data.fd == server.listen_socket) {
        /* accept connection */
        accept_conn_req(server);
      }
      else {
        process_event(server, events[n]);
      }
    }
  }

  /* clear all file descriptors from {events} */
  /*close(epoll_fd);
  close(listen_sock);*/
  printf("program finished successfully\n");

  return 0;
}



int create_listening_socket(unsigned short port) {
  int sock_fd, opt;
  unsigned int queue_size = 5;
  struct sockaddr_in addr;
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(sock_fd == -1) {
    perror("can't create a listening socket\n");
    /*cannot create socket */
    exit(EXIT_FAILURE);
  }

  opt = 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  if(bind(sock_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
    perror("listening socket binding error\n");
    exit(EXIT_FAILURE);
  }
  if(listen(sock_fd, queue_size) == -1) {
    perror("can't enable listening mode\n");
    exit(EXIT_FAILURE);
  }
  return sock_fd;
}

void accept_conn_req(struct _server server) {
  int new_fd;
  Client new_client;
  struct sockaddr_in addr;
  struct epoll_event ev;
  socklen_t  addr_len;

  addr_len = sizeof(addr);
  new_fd= accept(server.listen_socket, (struct sockaddr*) &addr, &addr_len);
  if(new_fd == -1) {
    perror("accept connection error\n");
    exit(EXIT_FAILURE);
  }
  set_nonblocking(new_fd);
  printf("new client address %hhu.%hhu.%hhu.%hhu:%hu\n",
    addr.sin_addr.s_addr,
    addr.sin_addr.s_addr >> 8,
    addr.sin_addr.s_addr >> 16,
    addr.sin_addr.s_addr >> 24,
    ntohs(addr.sin_port)
  );

  /* add socket to tracked by epoll */
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = new_fd;
  if(epoll_ctl(server.epoll_socket, EPOLL_CTL_ADD, new_fd, &ev) == -1) {
    /* epoll ctl error */
    exit(EXIT_FAILURE);
  }
  new_client = create_client(new_fd);
  if(darray_push(server.clients, &new_client) != DA_OK) {
    perror("accept connection: darray push error\n");
    exit(EXIT_FAILURE);
  }
}

size_t find_fd_index(Darray *a, int fd) {
  Client *client;
  size_t index;
  for(index = 0; index < a->size; index++) {
    client = darray_get(a, index);
    if(client != NULL && client->fd == fd) {
      break;
    }
  }
  return index;
}

void set_nonblocking(int ifd) {
  int flags;
  flags = fcntl(ifd, F_GETFL);
  fcntl(ifd, F_SETFL, flags | O_NONBLOCK);
}

void process_event(struct _server server, struct epoll_event event) {
  char buf[BUFFER_SIZE];
  ssize_t buf_size;

  if(event.events & EPOLLIN) {
    /* unsafe read */
    buf_size = read(event.data.fd, buf, BUFFER_SIZE);
    if(buf_size == -1) {
      perror("read error\n");
      exit(EXIT_FAILURE);
    }
    else if(buf_size == 0) {
      printf("Nothing to read\n");
      if(epoll_ctl(server.epoll_socket, EPOLL_CTL_DEL, event.data.fd, &event) == -1) {
        perror("error removing socket from epoll\n");
        exit(EXIT_FAILURE);
      }
      /* very ineffecient way of searching fds */
      darray_remove(server.clients, find_fd_index(server.clients, event.data.fd), NULL);
      close(event.data.fd);
    }
    else {
      /* unsafe write queue required */
      if(write(event.data.fd, buf, buf_size) == -1) {
        exit(EXIT_FAILURE);
      }
    }
  }
}
