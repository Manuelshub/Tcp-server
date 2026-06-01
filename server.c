#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define MAX_LENGTH 128
#define BACKLOG 10

int main() {
	int sock_fd, accept_fd, optval;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addrlen;
	char buffer[MAX_LENGTH];
	ssize_t bytes_read;
	const char *message = "Message received";

	printf("Welcome to Our Custom made Tcp Server\n");

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket failed");
		return EXIT_FAILURE;
	}
	optval = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		perror("setsockopt failed");
		return EXIT_FAILURE;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("bind failed");
		return EXIT_FAILURE;
	}

	if (listen(sock_fd, BACKLOG) == -1) {
		perror("listen failed");
		return EXIT_FAILURE;
	}

	addrlen = (socklen_t)sizeof(client_addr);
	accept_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
	if (accept_fd == -1) {
		perror("accept failed");
		return EXIT_FAILURE;
	}

	bytes_read = read(accept_fd, buffer, MAX_LENGTH - 1);
	if (bytes_read <= 0) {
		if (bytes_read == 0)
			fprintf(stderr, "Client disconnected\n");
		else
			perror("read failed");
		return EXIT_FAILURE;
	}
	buffer[bytes_read] = '\0';

	if (write(accept_fd, message, strlen(message)) == -1) {
		perror("Write to accepted conn failed");
		return EXIT_FAILURE;
	}

	if (write(1, buffer, bytes_read) == -1) {
		perror("write failed");
		return EXIT_FAILURE;
	}
	
	close(accept_fd);
	close(sock_fd);
	return EXIT_SUCCESS;
}
