#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <routing_table.h>
#include <utils.h>
#include <client_sync.h>

#define SOCK_PATH "/tmp/r-table"

int main(int argc, char const *argv[]) {
    const int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const sockaddr_un name = { AF_UNIX, SOCK_PATH };

    int ret = connect(sock, (const sockaddr *)&name, sizeof(sockaddr_un));
    if (ret == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    ClientSyncPayload payload;

    ret = read(sock, &payload, sizeof(ClientSyncPayload)); 
    if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    RoutingTable table;

    if (payload.action != C_SKIP) {
        RoutingTableEntry temp;
        int size = atoi(payload.payload_buf);
        for (int i = 0; i < size; i++) {
            memset(&temp, 0, sizeof(RoutingTableEntry));
            ret = read(sock, &temp, sizeof(RoutingTableEntry));
            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }       
            table.entries.emplace_back(temp);
        }
    }

    table.print();

    while (1) {
        memset(&payload, 0, sizeof(ClientSyncPayload));

        ret = read(sock, &payload, sizeof(ClientSyncPayload)); 
        if (ret == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (payload.action == C_EXIT) {
            break;
        } else if (payload.action == C_ADD) {
            add_new_entry(payload.payload_buf, table);
        } else if (payload.action == C_DELETE) {
            delete_entry(payload.payload_buf, table);
        } else if (payload.action == C_UPDATE) {
            update_entry(payload.payload_buf, table);
        }

        printf("\n");
        table.print();
    }

    close(sock);

    return 0;
}
