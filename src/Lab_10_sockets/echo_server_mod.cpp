/*
** echo_server_mod.cpp -- the echo server for echo_cient.cpp; demonstrates UNIX sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "echo_socket"

volatile int clientN = 0;

int main(void)
{
	int s, s2, t, len;
	struct sockaddr_un local, remote;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
//	remote.sun_family = AF_UNIX;       //
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	int pid = 0;
	for(;;) {
		int done, n;
        parent:
		printf("Waiting for a connection...\n");
		t = sizeof(remote);
		if ((s2 = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1) {
			perror("accept");
			exit(1);
		}

		printf("Client %d connected.\n", ++clientN);
        pid = fork();
        if (pid > 0) goto parent;
        else {
            done = 0;
            do {
                char str[100];
                n = recv(s2, str, 100, 0);
                if (n <= 0) {
                    if (n < 0) perror("recv");
                    done = 1;
                }

                printf("Client %d said: %s", clientN, str);

                if (!done)
                    if (send(s2, str, n, 0) < 0) {
                        perror("send");
                        done = 1;
                    }
                memset(str, 0, sizeof str);
            } while (!done);
            printf("Closing connection with client %d...\n", clientN);
        }

		close(s2);
        printf("Waiting for a connection...\n");
        break;
	}

	return 0;
}



