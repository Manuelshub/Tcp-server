#include "helper.h"

#define PORT 8080
#define MAX_LENGTH 512
#define BACKLOG 10
#define MAX_CLIENTS 20

int main() {
	int i, sock_fd, accept_fd, optval, poll_res;
	char buffer[MAX_LENGTH], *msg_to_user;
	socklen_t addrlen;
	struct sockaddr_in server_addr, client_addr;
	struct pollfd fds[MAX_CLIENTS + 1];
	ssize_t bytes_read;
	nfds_t nfds;

	printf("Welcome to Our Custom made Tcp Chat App\n");
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
	nfds = 1;
	fds[0].fd = sock_fd;
	fds[0].events = POLLIN;

	for (i = 1; i <= MAX_CLIENTS; i++) {
	    fds[i].fd = -1;
		fds[i].events = POLLIN;
	}
	msg_to_user = "Welcome onboard user!!!";

	while (1) {
		poll_res = poll(fds, nfds, -1);
		if (poll_res <= 0) {
		    if (poll_res == -1) {
				if (errno == EINTR) continue;
				perror("poll()");
				exit(EXIT_FAILURE);
			}
			continue;
		}
		if (fds[0].revents & POLLIN) {
		    accept_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
		    if (accept_fd == -1) {
		        if (errno == EINTR) continue;
				perror("accept()");
				continue;
		    }
			if (send(accept_fd, msg_to_user, strlen(msg_to_user), 0) == -1) {
			    close(accept_fd);
				return handle_error("send()");
			}
			for (i = 1; i <= MAX_CLIENTS; i++) {
				if (fds[i].fd == -1) {
					fds[i].fd = accept_fd;
					break;
				}
			}
			if (i == MAX_CLIENTS) {
			    fprintf(stderr, "Max number of clients reached\n");
			    close(accept_fd);
			}
			else {
			    if (i + 1 > (int)nfds)
					nfds = i + 1;
			}
		}
		for (i = 1; i < MAX_CLIENTS + 1; i++) {
		    if (fds[i].fd == -1) continue;
			if (fds[i].revents & POLLIN) {
				bytes_read = read(fds[i].fd, buffer, MAX_LENGTH-1);
				if (bytes_read <= 0) {
					if (bytes_read == 0)
						fprintf(stderr, "client %d left the chat\n", fds[i].fd);
					else
					    perror("read()");
					close(fds[i].fd);
					fds[i].fd = -1;
				}
				else {
				    buffer[bytes_read] = '\0';
					printf("[%s] %s\n", get_log_timestamp(), buffer);
					for (int k = 1; k < MAX_CLIENTS + 1; k++) {
					    if (fds[k].fd != -1 && fds[k].fd != fds[i].fd) {
							if (write(fds[k].fd, buffer, bytes_read) == -1) {
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
