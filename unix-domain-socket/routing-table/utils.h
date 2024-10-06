#pragma once

#include <routing_table.h>

template <typename T>
void fill_buf(T *buf, T val, int n) {
    for (int i = 0; i < n; i++) {
        buf[i] = val;
    }
}

size_t terminate_return(char *buf) {
    size_t len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return len - 1;
    }
    return len;
}

bool add_new_entry(char *buf, RoutingTable &table) {
    strtok(buf, " "); // skip operation code

    /**
     * strlen(char *) returns length excluding the null terminator character ('\0')
     * to fit the string within a bound the length should be -> length + 1(for '\0') <= bound 
     */

    char *destination = strtok(NULL, " ");
    if (destination == NULL || strlen(destination) >= IP_ADDR_LEN) { 
        return false;
    }

    char *mask = strtok(NULL, " ");
    if (mask == NULL || strlen(mask) >= MASK_LEN) {
        return false;
    }

    char *gateway_ip = strtok(NULL, " ");
    if (gateway_ip == NULL || strlen(gateway_ip) >= IP_ADDR_LEN) {
        return false;
    }

    char *oif = strtok(NULL, " ");
    if (oif == NULL || terminate_return(oif) >= O_IF_LEN) {
        return false;
    }

    return table.insert(destination, mask, gateway_ip, oif);
}

bool update_entry(char *buf, RoutingTable &table) {
    strtok(buf, " "); // skip operation code

    char *destination = strtok(NULL, " ");
    if (destination == NULL || strlen(destination) >= IP_ADDR_LEN) {
        return false;
    }

    char *mask = strtok(NULL, " ");
    if (mask == NULL || 0 > strlen(mask) >= MASK_LEN) {
        return false;
    }

    char *gateway_ip = strtok(NULL, " ");
    if (gateway_ip == NULL || strlen(gateway_ip) >= IP_ADDR_LEN) {
        return false;
    }

    char *oif = strtok(NULL, " ");
    if (oif == NULL || terminate_return(oif) >= O_IF_LEN) {
        return false;
    }

    EntryUpdatePayload payload(gateway_ip, oif);

    return table.update(destination, mask, payload);
}

bool delete_entry(char *buf, RoutingTable &table) {
    strtok(buf, " "); // skip operation code

    char *destination = strtok(NULL, " ");
    if (destination == NULL || strlen(destination) >= IP_ADDR_LEN) {
        return false;
    }

    char *mask = strtok(NULL, " ");
    if (mask == NULL || terminate_return(mask) >= MASK_LEN) {
        return false;
    }

    return table.remove(destination, mask);
}