#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

pthread_t p_read, p_write;

#define XDDP_PORT_SEND 3
#define XDDP_PORT_REC 2

static void fail(const char *reason)
{
    perror(reason);
    exit(EXIT_FAILURE);
}

void *read_xddp(void *fd_r)
{
    double received;
    int ret;
    int fd = *((int *)fd_r);
    for (;;)
    {
        /* Get the next message from realtime_thread2. */
        ret = read(fd, &received, sizeof(received));
        if (ret <= 0)
            fail("read");
        printf("received: %f\n", received);
    }
    return NULL;
}

void *write_xddp(void *fd_r)
{
    int ret;
    struct timespec ts;
    double toSend = 0.3;
    int fd = *((int *)fd_r);
    for (;;)
    {

        printf("sending: %f\n", toSend);
        ret = write(fd, &toSend, sizeof(toSend));
        if (ret <= 0)
            fail("write");

        ts.tv_sec = 0;
        ts.tv_nsec = 500000000; /* 500 ms */
        clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
    }
    return NULL;
}

void openport(int *port, int label, int ReadWrite)
{
    char buf[128], *devname;
    int ret, n = 0;
    if (asprintf(&devname, "/dev/rtp%d", label) < 0)
        printf("asprintf");
    *port = open(devname, O_RDWR);
    free(devname);
    if (port < 0)
        fail("open");
}

int main(int argc, char **argv)
{
    struct timespec ts;
    struct timeval tv;
    double testdouble = 5.1231;
    int testint = 7328;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int fd_send, fd_rec, ret;
    printf("begin opening ports...\n");
    openport(&fd_send, XDDP_PORT_SEND, O_RDONLY);
    openport(&fd_rec, XDDP_PORT_REC, O_WRONLY);
    printf("begin creating pthreads...\n");
    ret = pthread_create(&p_read, NULL, read_xddp, (void *)&fd_send);
    ret = pthread_create(&p_write, NULL, write_xddp, (void *)&fd_rec);
    sleep(1);
    printf("KLAARRR!");
    close(fd_send);
    close(fd_rec);
    return 0;
}