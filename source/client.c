#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main() {
	int sfd, opt;
	struct sockaddr_in addr;
	socklen_t size;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1) {
		/* cannot create socket */
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
		printf("error\n");
		/* connection error */
		exit(EXIT_FAILURE);
	}
	sleep(10);

	close(sfd);
	return 0;
}
