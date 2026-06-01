#include "helper.h"

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
	pid_t child_pid;

	printf("Welcome to Our Custom made Tcp Server\n");
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		return handle_error("socket()");
	}
	optval = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		return handle_error("setsockopt()");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
		return handle_error("bind()");
	}

	if (listen(sock_fd, BACKLOG) == -1) {
		return handle_error("listen()");
	}

	addrlen = (socklen_t)sizeof(client_addr);
	signal(SIGCHLD, sigchld_handler);
	
	while (1) {
		accept_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
		if (accept_fd == -1) {
		    if (errno == EINTR) {
		        continue;
		    }
			return handle_error("accept()");
		}

		child_pid = fork();
		if (child_pid == -1) {
		    return handle_error("fork()");
		}
		if (child_pid == 0) {
			close(sock_fd);
			bytes_read = read(accept_fd, buffer, MAX_LENGTH - 1);
			if (bytes_read <= 0) {
				if (bytes_read == 0)
					fprintf(stderr, "Client disconnected\n");
				else
					perror("read failed");
				close(accept_fd);
				_exit(EXIT_FAILURE);
			}
			buffer[bytes_read] = '\0';

			if (write(accept_fd, message, strlen(message)) == -1) {
				handle_error("Write to accepted conn failed");
			    close(accept_fd);
				_exit(EXIT_FAILURE);
			}

			if (write(1, buffer, bytes_read) == -1) {
				handle_error("write failed");
				close(accept_fd);
				_exit(EXIT_FAILURE);
			}
			close(accept_fd);
			_exit(EXIT_SUCCESS);
		}
		close(accept_fd);
	}
	/* Unreachable code, loop exits only via signal */
	/* close(sock_fd);
	return EXIT_SUCCESS; */
}
