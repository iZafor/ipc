#pragma once

#include <routing_table.h>

enum ClientSyncAction {
    C_ADD,
    C_DELETE,
    C_UPDATE,
    C_EXIT,
    C_SKIP,
    C_CONTINUE
};

struct ClientSyncPayload {
    ClientSyncAction action;
    char payload_buf[BUF_SIZE];

    ClientSyncPayload() = default;

    ClientSyncPayload(ClientSyncAction action, char payload_buf[BUF_SIZE]) {
        this->action = action;
        memmove(this->payload_buf, payload_buf, BUF_SIZE);
    }
};