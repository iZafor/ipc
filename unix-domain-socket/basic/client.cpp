#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/infant-server"

int main(int argc, char const *argv[]) {
    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const sockaddr_un name = { AF_UNIX, SOCKET_PATH };

    int ret = connect(client_sock, (const sockaddr *)&name, sizeof(sockaddr_un));
    if (ret == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    } 

    int data[] = {1, 2, 3, 0};
    for (int i = 0; i < sizeof(data) / sizeof(int); i++) {
        ret = write(client_sock, data + i, sizeof(int));
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    int sum = 0;
    ret = read(client_sock, &sum, sizeof(int));
    if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Summation = %d\n", sum);

    close(client_sock);    

    return 0;
}
