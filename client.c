#include "helper.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 128

int main(int ac, char **av) {
	int sock_fd, pton_res;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE], client_name[32];
	ssize_t send_res, recv_res;
	fd_set read_fds;

	if (ac != 2) {
	    fprintf(stderr, "Usage: %s <client_name>\n", av[0]);
		return EXIT_FAILURE;
	}
	snprintf(client_name, sizeof(client_name), "%s", av[1]);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket failed");
		return EXIT_FAILURE;
	}

	memset(&addr, 0, sizeof(addr));	
	pton_res = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if (pton_res <= 0) {
		if (pton_res == 0)
			fprintf(stderr, "Not in presentation format\n");
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
	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(sock_fd, &read_fds);
		FD_SET(0, &read_fds);
		if (select(sock_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			return handle_error("select()");
		}
		if (FD_ISSET(0, &read_fds)) {
		    char outbuf[BUFFER_SIZE + 32 + 4];
			if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)  break;
			buffer[strcspn(buffer, "\n")] = '\0';
			snprintf(outbuf, sizeof(outbuf), "[%s] %s", client_name, buffer);
			send_res = send(sock_fd, outbuf, strlen(outbuf), 0);
			if (send_res == -1) return handle_error("send()");
		}
		if (FD_ISSET(sock_fd, &read_fds)) {
			recv_res = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
			if (recv_res <= 0) {
				if (recv_res == 0)
					fprintf(stderr, "Connection closed by server\n");
				else
					perror("recv");
				return EXIT_FAILURE;
			}
			buffer[recv_res] = '\0';
			printf("%s\n", buffer);
		}
	}

	close(sock_fd);
	return EXIT_SUCCESS;
}
