#pragma once

#include <string.h>
#include <memory.h>

#define SHOW_LEN 4
#define ADD_LEN 3
#define DELETE_LEN 6
#define UPDATE_LEN 6
#define EXIT_LEN 4

enum ServerAction {
    SA_SHOW,
    SA_ADD,
    SA_DELETE,
    SA_UPDATE,
    SA_EXIT,
    SA_SKIP
};

ServerAction get_action(char *buf) {
    int len = strlen(buf);

    if (len < 3 || 
        (memcmp(buf, "S", 1) &&
        memcmp(buf, "A", 1) &&
        memcmp(buf, "D", 1) &&
        memcmp(buf, "U", 1) &&
        memcmp(buf, "E", 1)) 
    ) {
        return SA_SKIP;
    }

    if (memcmp(buf, "SHOW", SHOW_LEN) == 0) {
        return SA_SHOW;
    } else if (memcmp(buf, "ADD", ADD_LEN) == 0) {
        return SA_ADD;
    } else if (memcmp(buf, "DELETE", DELETE_LEN) == 0) {
        return SA_DELETE;
    } else if (memcmp(buf, "UPDATE", UPDATE_LEN) == 0) {
        return SA_UPDATE;
    } else if (memcmp(buf, "EXIT", EXIT_LEN) == 0) {
        return SA_EXIT;
    } else {
        return SA_SKIP;
    }
}
