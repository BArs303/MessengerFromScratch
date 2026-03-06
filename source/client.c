#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main() {
	int sfd, opt;
	struct sockaddr_in addr;
	socklen_t size;
	char buf[1024];
	size_t buf_size;

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

	printf("You can send 3 messages\n");
	for(int i = 0; i < 3; i++) {
		buf_size = read(STDIN_FILENO, buf, 1024);
		buf[buf_size - 1] = '\0';

		if(buf_size == -1) {
			exit(EXIT_FAILURE);
		}
		else if(buf_size == 0) {
			break;
		}
		else {
			if(write(sfd, buf, buf_size) == -1) {
				exit(EXIT_FAILURE);
			}
			buf_size = read(sfd, buf, buf_size);
			printf("Server response: %s\n", buf);
		}
	}

	close(sfd);
	return 0;
}
