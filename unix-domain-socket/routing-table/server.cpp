#include <multiplexed_server.h>
#include <routing_table.h>
#include <server_action.h>
#include <utils.h>
#include <client_sync.h>

#define SOCK_PATH "/tmp/r-table"

int main(int argc, char const *argv[]) {
    MultiplexedServer server(SOCK_PATH);
    RoutingTable table;
    char std_in_buf[BUF_SIZE]; 
    char copy_buf[BUF_SIZE];
    char dummy_buf[0];

    while(1) {
        memset(std_in_buf, 0, BUF_SIZE);

        printf("*********************************************************************************\n");
        printf("* Select operation                                                              *\n");
        printf("* SHOW - to show entries                                                        *\n");
        printf("* ADD <destination><space><mask><space><gateway ip><space><output interface>    *\n");
        printf("* DELETE <destination><space><mask>                                             *\n");
        printf("* UPDATE <destination><space><mask><space><gateway ip><space><output interface> *\n");
        printf("* EXIT - to exit server                                                         *\n");
        printf("*********************************************************************************\n");

        int active_sock = server.select_fd();
        if (active_sock == server.get_conn_sock_fd()) {
            int data_sock = accept(server.get_conn_sock_fd(), NULL, NULL);
            if (data_sock == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            server.insert_fd(data_sock);

            if (table.size() > 0) {
                char size_buf[10];
                sprintf(size_buf, "%d", table.size());

                ClientSyncPayload payload(C_CONTINUE, size_buf);
                write(data_sock, &payload, sizeof(ClientSyncPayload));

                for (auto entry: table.entries) {
                    write(data_sock, &entry, sizeof(RoutingTableEntry));
                }
            } else {
                ClientSyncPayload payload(C_SKIP, dummy_buf);
                write(data_sock, &payload, sizeof(ClientSyncPayload));
            }
        } else if (active_sock == STDIN_FILENO) {
            read(0, std_in_buf, BUF_SIZE);

            ServerAction action = get_action(std_in_buf);
            switch (action) {
                case SA_SKIP:
                    continue;
                case SA_EXIT:
                    server.send_payload_to_all_clients(ClientSyncPayload(C_EXIT, std_in_buf));
                    goto end;
                case SA_SHOW:
                    table.print();
                    break;
                case SA_ADD:
                    memset(copy_buf, 0, BUF_SIZE);
                    strcpy(copy_buf, std_in_buf);

                    if (add_new_entry(std_in_buf, table)) {
                        server.send_payload_to_all_clients(ClientSyncPayload(C_ADD, copy_buf));
                    }
                    break;
                case SA_UPDATE:
                    memset(copy_buf, 0, BUF_SIZE);
                    strcpy(copy_buf, std_in_buf);

                    if (update_entry(std_in_buf, table)) {
                        server.send_payload_to_all_clients(ClientSyncPayload(C_UPDATE, copy_buf));
                    }
                    break;
                case SA_DELETE:
                    memset(copy_buf, 0, BUF_SIZE);
                    strcpy(copy_buf, std_in_buf);

                    if (delete_entry(std_in_buf, table)) {
                        server.send_payload_to_all_clients(ClientSyncPayload(C_DELETE, copy_buf));
                    }
                    break;
            }
        }
    }

    end: 
        return 0;
}
