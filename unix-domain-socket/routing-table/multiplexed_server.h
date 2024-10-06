#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <utils.h>

struct MultiplexedServer {
    MultiplexedServer(char const *socket_path, const __socket_type socket_type = SOCK_STREAM, const int n_clients = 10) {
        unlink(socket_path);

        conn_sock = socket(AF_UNIX, socket_type, 0);
        if (conn_sock == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        printf("new socket created\n");

        name.sun_family = AF_UNIX;
        strcpy(name.sun_path, socket_path);

        int ret = bind(conn_sock, (const sockaddr *)&name, sizeof(name));
        if (ret == -1) {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        printf("socket bound to path %s\n", socket_path);

        ret = listen(conn_sock, n_clients);
        if (ret == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        printf("socket is listening for %d client requests at a time\n", n_clients);

        count = n_clients + 2; // 2 = stdin + connection socket

        monitored_fds = new int[count];
        monitored_fds[0] = STDIN_FILENO; // standard input
        monitored_fds[1] = conn_sock; // connection socket
        fill_buf(monitored_fds + 2, -1, n_clients);
    }

    ~MultiplexedServer() {
        printf("closing server\n");
        close(conn_sock);
        unlink(name.sun_path);
        delete monitored_fds;
    }

    void insert_fd(int fd) {
        for (int i = 0; i < count; i++) {
            if (monitored_fds[i] == -1) {
                monitored_fds[i] = fd;
                return;
            }
        }        
    }

    void remove_fd(int fd) {
        for (int i = 0; i < count; i++) {
            if (monitored_fds[i] == fd) {
                monitored_fds[i] = -1;
                return;
            }
        }
    }

    int get_conn_sock_fd() const {
        return conn_sock;
    }

    int select_fd() {
        refresh_fd_set();

        int ret = select(get_max_fd() + 1, &read_fds, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        return get_active_fd();
    }

    template<typename _Payload>
    void send_payload_to_all_clients(_Payload &&payload) {
        for (int i = 2; i < count; i++) {
            if (monitored_fds[i] != -1) {
                write(monitored_fds[i], &payload, sizeof(_Payload));
            }
        }
    }

    private:
        int conn_sock;
        sockaddr_un name;
        int *monitored_fds;
        fd_set read_fds;
        int count;

        int get_max_fd() const {
            int max = -1;
            for (int i = 0; i < count; i++) {
                if (monitored_fds[i] > max) {
                    max = monitored_fds[i];
                }
            }
            return max;
        }

        void refresh_fd_set() {
            FD_ZERO(&read_fds);

            for (int i = 0; i < count; i++) {
                if (monitored_fds[i] != -1) {
                    FD_SET(monitored_fds[i], &read_fds);
                }
            }        
        }

        int get_active_fd() const {
            for (int i = 0; i < count; i++) {
                if (FD_ISSET(monitored_fds[i], &read_fds)) {
                    return monitored_fds[i];
                }   
            }
            return -1;
        }
};
