#include <stdio.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define QUEUE_ID "/mqueue-basic"
#define MQ_MAXMSG 10
#define MQ_MSGSIZE 1024
#define Q_PERMISSIONS 0660
#define BUF_SIZE MQ_MSGSIZE

int main(int argc, char const *argv[]) {
    mq_attr attr;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;

    mqd_t mq_fd = mq_open(QUEUE_ID, O_RDONLY, Q_PERMISSIONS, &attr);
    if (mq_fd == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Opened message queue %s\n", QUEUE_ID);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    if (mq_receive(mq_fd, buf, BUF_SIZE, 0) == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    printf("Received message: %s\n", buf);

    mq_close(mq_fd);

    return 0;
}
