#include "helper.h"

#define PORT 8080
#define BUFFER_SIZE 512

int main(int ac, char **av) {
	int sock_fd, pton_res, poll_res;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE], *client_name, client_msg[BUFFER_SIZE];
	ssize_t send_res, recv_res;
	struct pollfd fds[2];

	if (ac != 2) {
	    fprintf(stderr, "Usage: %s <client_name>\n", av[0]);
		return EXIT_FAILURE;
	}
	client_name = malloc(strlen(av[1]) + 1);
	if (client_name == NULL) {
	    fprintf(stderr, "malloc failed\n");
		return EXIT_FAILURE;
	}
	strcpy(client_name, av[1]);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket failed");
		free(client_name);
		return EXIT_FAILURE;
	}

	memset(&addr, 0, sizeof(addr));	
	pton_res = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if (pton_res <= 0) {
		if (pton_res == 0)
			fprintf(stderr, "Not in presentation format\n");
		else
			perror("inet_pton");
		free(client_name);
		return EXIT_FAILURE;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);

	if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect");
		free(client_name);
		close(sock_fd);
		return EXIT_FAILURE;
	}
	
	snprintf(client_msg, sizeof(client_msg), "%s Joined the chat!!!", client_name);
	if (send(sock_fd, client_msg, strlen(client_msg), 0) == -1) {
	    free(client_name);
		return handle_error("send()");
	}

	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[1].fd = sock_fd;
	fds[1].events = POLLIN;
	while (1) {
		poll_res = poll(fds, 2, -1);
		if (poll_res <= 0) {
		    if (poll_res == -1) {
				if (errno == EINTR) continue;
				free(client_name);
				perror("poll()");
				exit(EXIT_FAILURE);
			}
			continue;
		}
		if (fds[0].revents & POLLIN) {
		    char outbuf[BUFFER_SIZE + 64];
			if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)  break;
			buffer[strcspn(buffer, "\n")] = '\0';
			snprintf(outbuf, sizeof(outbuf), "[%s] %s", client_name, buffer);
			send_res = send(sock_fd, outbuf, strlen(outbuf), 0);
			if (send_res == -1) {
			    free(client_name);
				return handle_error("send()");
			}
		}
		if (fds[1].revents & POLLIN) {
			recv_res = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
			if (recv_res <= 0) {
				if (recv_res == 0)
					fprintf(stderr, "Connection closed by server\n");
				else
					perror("recv");
				free(client_name);
				return EXIT_FAILURE;
			}
			buffer[recv_res] = '\0';
			printf("%s\n", buffer);
		}
	}

	free(client_name);
	close(sock_fd);
	return EXIT_SUCCESS;
}
