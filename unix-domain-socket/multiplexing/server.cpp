#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mul"
#define MAX_CLIENTS 10
#define MAX_FDS MAX_CLIENTS + 1

int monitored_fds[MAX_FDS];

template<typename T>
void buf_fill(T *buf, T val, size_t count);

void initialize_monitored_fds();
void insert_into_monitored_fds(int fd);
void remove_from_monitored_fds(int fd);
void refresh_fd_set(fd_set *fdset_p);
int get_max_fd();
void exit_on_error(const char *message);

int main(int argc, char const *argv[]) {
    unlink(SOCK_PATH);

    const int conn_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (conn_sock == -1) {
        exit_on_error("socket");
    }

    const sockaddr_un name = { AF_UNIX, SOCK_PATH };
    int ret = bind(conn_sock, (const sockaddr *)&name, sizeof(sockaddr_un));
    if (ret == -1) {
        exit_on_error("bind");
    }
    
    ret = listen(conn_sock, MAX_CLIENTS);
    if (ret == -1) {
        exit_on_error("listen");
    }
    
    initialize_monitored_fds();
    insert_into_monitored_fds(conn_sock);

    int buf = 0;
    fd_set read_fds;
    int client_data[MAX_CLIENTS];
    buf_fill(client_data, 0, MAX_CLIENTS);

    while (1) {
        refresh_fd_set(&read_fds);

        ret = select(get_max_fd() + 1, &read_fds, NULL, NULL, NULL);
        if (ret == -1) {
            exit_on_error("select");
        }

        if (FD_ISSET(conn_sock, &read_fds)) {
            int data_sock = accept(conn_sock, NULL, NULL);
            if (data_sock == -1) {
                exit_on_error("accept");
            }
            insert_into_monitored_fds(data_sock);
        } else {
            for (int i = 1; i <= MAX_CLIENTS; i++) {
                if (FD_ISSET(monitored_fds[i], &read_fds)) {
                    int data_sock = monitored_fds[i];

                    ret = read(data_sock, &buf, sizeof(int));
                    if (ret == -1) {
                        exit_on_error("read");
                    }

                    if (buf == 0) {
                        ret = write(data_sock, &client_data[i - 1], sizeof(int));
                        if (ret == -1) {
                            exit_on_error("write");
                        }
                        client_data[i - 1] = 0;
                        close(data_sock);
                        remove_from_monitored_fds(data_sock);
                    } else {
                        client_data[i - 1] += buf;
                    }

                    buf = 0;

                    break;
                }
            }            
        }
    }
    
    end:
        close(conn_sock);
        unlink(SOCK_PATH);

    return 0;
}

template<typename T>
void buf_fill(T *buf, T val, size_t count) {
    for (size_t i = 0; i < count; i++) {
        buf[i] = val;
    }
}

void initialize_monitored_fds() {
    buf_fill(monitored_fds, -1, MAX_FDS);
}

void insert_into_monitored_fds(int fd) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (monitored_fds[i] == -1) {
            monitored_fds[i] = fd;
            return;
        }
    }
}

void remove_from_monitored_fds(int fd) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (monitored_fds[i] == fd) {
            monitored_fds[i] = -1;
            return;
        }
    }
}

void refresh_fd_set(fd_set *fdset_p) {
    FD_ZERO(fdset_p);

    for (int i = 0; i < MAX_FDS; i++) {
        if (monitored_fds[i] != -1) {
            FD_SET(monitored_fds[i], fdset_p);
        }
    }
}

int get_max_fd() {
    int res = -1;
    for (int i = 0; i < MAX_FDS; i++) {
        if (monitored_fds[i] > res) {
            res = monitored_fds[i];
        }    
    }
    return res;
}

__always_inline void exit_on_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}