#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mul"

void exit_on_error(const char *message);

int main(int argc, char const *argv[]) {
    const int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        exit_on_error("socket");
    }

    const sockaddr_un name = { AF_UNIX, SOCK_PATH };

    int ret = connect(sock, (const sockaddr *)&name, sizeof(sockaddr_un));
    if (ret == -1) {
        exit_on_error("connect");
    }

    int n = -1;

    while (n) {
        scanf("%d", &n);
        
        ret = write(sock, &n, sizeof(int));
        if (ret == -1) {
            exit_on_error("write");
        }
    }

    ret = read(sock, &n, sizeof(int));
    if (ret == -1) {
        exit_on_error("write");
    }

    printf("Summation: %d\n", n);

    close(sock);

    return 0;
}

__always_inline void exit_on_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}