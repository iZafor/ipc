#include <stdio.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_ID "/mqueue-basic"
#define MQ_MAXMSG 10
#define MQ_MSGSIZE 1024
#define Q_PERMISSIONS 0660
#define BUF_SIZE MQ_MSGSIZE

int main(int argc, char const *argv[]) {
    mq_attr attr;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;

    mqd_t mq_fd = mq_open(QUEUE_ID, O_CREAT | O_CLOEXEC | O_WRONLY, Q_PERMISSIONS, &attr);
    if (mq_fd == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("New message queue created.\n");

    if (mq_getattr(mq_fd, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    printf("Enter message: ");
    scanf("%s", buf);

    if (mq_send(mq_fd, buf, strlen(buf) + 1, 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    mq_close(mq_fd);
    mq_unlink(QUEUE_ID);

    return 0;
}
