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

#define MAX_EVENTS 10

int create_listening_socket(unsigned short port, int listen_queue);
void accept_conn_req(int efd, int listen_sock);
void set_nonblocking(int ifd);

int main() 
{
	int listen_sock, epoll_fd, nfds;
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];

	epoll_fd = epoll_create1(0);
	if(epoll_fd == -1) {
		/* epoll create error */
		exit(EXIT_FAILURE);
	}

	listen_sock = create_listening_socket(9997, 5);
	printf("listen socket %d\n", listen_sock);

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		/* epoll ctl error */
		exit(EXIT_FAILURE);
	}
	for(;;) {
		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(nfds == -1) {
			/* epoll_wait error */
			exit(EXIT_FAILURE);
		}
		printf("%d\n", nfds);
		for(int n = 0; n < nfds; n++) {
			if(events[n].data.fd == listen_sock) {
				/* accept connection */
				accept_conn_req(epoll_fd, listen_sock);
			}
			else {
				/* process data */
			}
		}
	}

	/* clear all file descriptors from {events} */
	close(epoll_fd);
	close(listen_sock);
	printf("program finished successfully\n");
	return 0;
}

int create_listening_socket(unsigned short port, int listen_queue)
{
	int sock_fd, opt;
	struct sockaddr_in addr;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		/*cannot create socket */
		exit(EXIT_FAILURE);
	}

	opt = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0x0;
	addr.sin_port = htons(port);
	if(bind(sock_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		/* cannot bind socket */
		exit(EXIT_FAILURE);
	}
	if(listen(sock_fd, listen_queue) == -1) {
		/* cannot enable listening mode */
		exit(EXIT_FAILURE);
	}
	return sock_fd;
}

void accept_conn_req(int efd, int listen_sock) {
	int new_sock;
	struct sockaddr_in addr;
	struct epoll_event ev;
	socklen_t  addr_len;
	
	addr_len = sizeof(addr);
	new_sock = accept(listen_sock, (struct sockaddr*) &addr, &addr_len);
	if(new_sock == -1) {
		/* cannot accept connection */
		exit(EXIT_FAILURE);
	}
	set_nonblocking(new_sock);
	printf("client address %hhu.%hhu.%hhu.%hhu:%hu\n",
			addr.sin_addr.s_addr,
			addr.sin_addr.s_addr >> 8,
			addr.sin_addr.s_addr >> 16,
			addr.sin_addr.s_addr >> 24,
			ntohs(addr.sin_port)
			);

	/* add socket to tracked by epoll */
	ev.events = EPOLLIN; //add all possible signals
	ev.data.fd = new_sock;
	if(epoll_ctl(efd, EPOLL_CTL_ADD, new_sock, &ev) == -1) {
		/* epoll ctl error */
		exit(EXIT_FAILURE);
	}
}

void set_nonblocking(int ifd) {
	int flags;
	flags = fcntl(ifd, F_GETFL);
	fcntl(ifd, F_SETFL, flags | O_NONBLOCK);
}
