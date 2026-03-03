#include <stdlib.h>

/* socket, bind, listen */
#include <sys/socket.h>

/* sockaddr_in */
#include <netinet/in.h>

/* for printf */
#include <stdio.h>

int create_listening_socket(unsigned short port, int listen_queue);

int main() 
{
	create_listening_socket(9997, 0);
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
		exit(-1);
	}

	opt = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0x0;
	addr.sin_port = htons(port);
	if(bind(sock_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		/* cannot bind socket */
		exit(-1);
	}
	if(listen(sock_fd, listen_queue) == -1) {
		/* cannot enable listening mode */
		exit(-1);
	}
	return sock_fd;
}
