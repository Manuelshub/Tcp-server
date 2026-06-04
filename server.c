#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#define PORT 8080
#define MAX_LENGTH 128
#define BACKLOG 10
#define MAX_CLIENTS 16

int main() {
	int i, sock_fd, accept_fd, optval, client_arr[MAX_CLIENTS];
	int max_fd;
	// const char *message = "Message received";
	char buffer[MAX_LENGTH];
	socklen_t addrlen;
	struct sockaddr_in server_addr, client_addr;
	ssize_t bytes_read;
	fd_set master_fds, temp_fds;

	printf("Welcome to Our Custom made Tcp Server\n");
	/* Opening up the socket to for incoming connections */
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

	/* Binding the socket to the server address */
	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
		return handle_error("bind()");
	}
	/* Listening for incoming connections */
	if (listen(sock_fd, BACKLOG) == -1) {
		return handle_error("listen()");
	}

	addrlen = (socklen_t)sizeof(client_addr);
	/* Initialize client array with -1 which is not a valid file descriptor */
	for (i = 0; i < MAX_CLIENTS; i++) {
		client_arr[i] = -1;
	}

	max_fd = sock_fd;
	FD_ZERO(&master_fds);
	FD_SET(sock_fd, &master_fds);
	while (1) {
	    temp_fds = master_fds;
		if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) == -1) {
			return handle_error("select()");
		}
		if (FD_ISSET(sock_fd, &temp_fds)) {
		    accept_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
		    if (accept_fd == -1) {
		        if (errno == EINTR) continue;
				perror("accept()");
				continue;
		    }
			FD_SET(accept_fd, &master_fds);
			if (accept_fd > max_fd)
				max_fd = accept_fd;
			for (i = 0; i < MAX_CLIENTS; i++) {
				if (client_arr[i] == -1) {
					client_arr[i] = accept_fd;
					break;
				}
			}
			if (i == MAX_CLIENTS) {
			    fprintf(stderr, "Max clients reached\n");
			    close(accept_fd);
			    FD_CLR(accept_fd, &master_fds);
			}
		}
		for (i = 0; i < MAX_CLIENTS; i++) {
		    if (client_arr[i] == -1) continue;
			if (FD_ISSET(client_arr[i], &temp_fds)) {
				bytes_read = read(client_arr[i], buffer, MAX_LENGTH-1);
				if (bytes_read <= 0) {
					if (bytes_read == 0)
						fprintf(stderr, "client %d disconnected\n", client_arr[i]);
					else
					    perror("read()");
					close(client_arr[i]);
					FD_CLR(client_arr[i], &master_fds);
					client_arr[i] = -1;
					max_fd = sock_fd;
					for (int j = 0; j < MAX_CLIENTS; j++) {
						if (client_arr[j] > max_fd)
							max_fd = client_arr[j];
					}
				}
				else {
				    buffer[bytes_read] = '\0';
					printf("[%s] %s\n", get_log_timestamp(), buffer);
					for (int k = 0; k < MAX_CLIENTS; k++) {
					    if (client_arr[k] != -1 && client_arr[k] != client_arr[i]) {
							if (write(client_arr[k], buffer, bytes_read) == -1) {
								perror("write()");
							}
						}
					}
				}
			}
		}
	}
	
	close(sock_fd);
	return EXIT_SUCCESS;
}
