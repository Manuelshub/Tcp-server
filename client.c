#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 128

int main(void) {
	int sock_fd, pton_res;
	struct sockaddr_in addr;
	const char *message = "Yo!! Accept connection\n";
	char buffer[BUFFER_SIZE];
	ssize_t send_res, recv_res;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket failed");
		return EXIT_FAILURE;
	}

	memset(&addr, 0, sizeof(addr));	
	pton_res = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if (pton_res <= 0) {
		if (pton_res == 0)
			fprintf(stderr, "Not in presentation format");
		else
			perror("inet_pton");
		return EXIT_FAILURE;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);

	if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect");
		return EXIT_FAILURE;
	}

	send_res = send(sock_fd, message, strlen(message), 0);
	if (send_res == -1) {
		perror("send");
		return EXIT_FAILURE;
	}
	
	recv_res = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
	if (recv_res <= 0) {
		if (recv_res == 0)
			fprintf(stderr, "Connection closed by server\n");
		else
			perror("recv");
		return EXIT_FAILURE;
	}
	buffer[recv_res] = '\0';
	printf("Received from server: %s\n", buffer);

	close(sock_fd);
	return EXIT_SUCCESS;
}
