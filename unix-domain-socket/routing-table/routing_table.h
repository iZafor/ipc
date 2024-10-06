#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <algorithm>
#include <vector>

// including '\0'
#define IP_ADDR_LEN 16
#define O_IF_LEN 32
#define MASK_LEN 3

#define BUF_SIZE 75

void str_move(char *dst, char *src, size_t len) {
    memmove(dst, src, len);
    dst[len] = '\0';
}

struct EntryUpdatePayload {
    char gateway_ip[IP_ADDR_LEN];
    char oif[O_IF_LEN];

    EntryUpdatePayload(char gateway_ip[IP_ADDR_LEN], char oif[O_IF_LEN]) {
        str_move(this->gateway_ip, gateway_ip, strlen(gateway_ip));
        str_move(this->oif, oif, strlen(oif));
    }
};

struct RoutingTableEntry {
    RoutingTableEntry() = default;

    RoutingTableEntry(char destination[IP_ADDR_LEN], char mask[MASK_LEN], char gateway_ip[IP_ADDR_LEN], char oif[O_IF_LEN]) {
        str_move(this->destination, destination, strlen(destination));
        str_move(this->mask, mask, strlen(mask));
        str_move(this->gateway_ip, gateway_ip, strlen(gateway_ip));
        str_move(this->oif, oif, strlen(oif));
    }

    void update(EntryUpdatePayload &payload) {
        memset(gateway_ip, 0, IP_ADDR_LEN);
        memset(oif, 0, IP_ADDR_LEN);

        str_move(gateway_ip, payload.gateway_ip, strlen(payload.gateway_ip));
        str_move(oif, payload.oif, strlen(payload.oif));
    }

    bool is_equal(char destination[IP_ADDR_LEN], char mask[MASK_LEN]) {
        return memcmp(this->destination, destination, strlen(destination)) == 0 && memcmp(this->mask, mask, strlen(mask)) == 0;
    }

    char *to_string() {
        char *buf = (char *)malloc(BUF_SIZE); 
        sprintf(buf, "%s/%s %s %s", destination, mask, gateway_ip, oif);
        return buf;
    }

    private:
        char destination[IP_ADDR_LEN];
        char mask[MASK_LEN];
        char gateway_ip[IP_ADDR_LEN];
        char oif[O_IF_LEN];
};

struct RoutingTable {
    std::vector<RoutingTableEntry> entries;

    RoutingTable() {
        entries.reserve(10);
    }

    ~RoutingTable() {
        entries.clear();
    }

    size_t size() {
        return entries.size();
    }

    std::vector<RoutingTableEntry>::iterator find(char destination[IP_ADDR_LEN], char mask[MASK_LEN]) {
        return std::find_if(
            entries.begin(),
            entries.end(),
            [destination, mask](RoutingTableEntry &entry) { return entry.is_equal(destination, mask); }
        );
    }

    bool insert(char destination[IP_ADDR_LEN], char mask[MASK_LEN], char gateway_ip[IP_ADDR_LEN], char oif[O_IF_LEN]) {
        if (find(destination, mask) != entries.end()) {
            return false;
        }
        entries.emplace_back(destination, mask, gateway_ip, oif);
        return true;
    }

    bool remove(char destination[IP_ADDR_LEN], char mask[MASK_LEN]) {
        auto entry = find(destination, mask);
        if (entry != entries.end()) {
            entries.erase(entry);
            return true;
        }
        return false;
    }

    bool update(char destination[IP_ADDR_LEN], char mask[MASK_LEN], EntryUpdatePayload &payload) {
        auto entry = find(destination, mask);
        if (entry != entries.end()) {
            entry->update(payload);
            return true;
        }
        return false;
    }

    void print() {
        for(auto entry: entries) {
            printf("%s\n", entry.to_string());
        }
    }
};
