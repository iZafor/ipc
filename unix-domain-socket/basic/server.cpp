#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/infant-server"

int main(int argc, char const *argv[]) {
    // remove the socket if it already exists
    unlink(SOCKET_PATH);

    int conn_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (conn_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    } 
    printf("Master socket created.\n");

    const sockaddr_un name = { AF_UNIX, SOCKET_PATH };

    // bind is used to tell the os to forward any client requests to the
    // given socket name
    int ret = bind(conn_sock, (const sockaddr *)&name, sizeof(sockaddr_un));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("bind successful.\n");

    ret = listen(conn_sock, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("listening.\n");

    int sum = 0;
    int buf = 0;

    while (1) {
        // accept Connection Initiation Request 
        int data_sock = accept(conn_sock, NULL, NULL);
        if (data_sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New client handle created.\n");

        while (1) {
            // reset the buffer
            memset(&buf, 0, sizeof(int));

            // wait for Service Request Messages
            ret = read(data_sock, &buf, sizeof(int));
            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("Data received.\n");

            if (buf == 0) break;
            sum += buf;
        }

        // send the data
        ret = write(data_sock, &sum, sizeof(int));
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        printf("Data sent.\n");

        // close Data Socket
        close(data_sock);

        break;
    }

    // close Connection Socket
    close(conn_sock);
    printf("Connection socket closed.\n");

    // remove the socket
    unlink(SOCKET_PATH);

    return 0;
}
