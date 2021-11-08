#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

pthread_t p_read, p_write;

#define XDDP_PORT_SEND 20
#define XDDP_PORT_REC 10

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
        std::ofstream myfile;
        myfile.open("timing.csv");
        myfile << "Timing\n";
        for (;;)
        {
                /* Get the next message from realtime_thread2. */
                ret = read(fd, &received, sizeof(received));
                if (ret <= 0)
                        fail("read");
                printf("received: %.15g\n", received);
                myfile << std::setprecision(std::numeric_limits<double>::digits10 + 1) << received << "\n";
                if (received > 5)
                {
                        break;
                }
        }
        myfile.close();
        return NULL;
}

void *write_xddp(void *fd_r)
{
        int ret;
        struct timespec ts;
        double toSend = 0.0;
        int fd = *((int *)fd_r);
        for (;;)
        {

                // toSend = toSend + 0.3;
                // printf("sending: %f\n", toSend);
                // ret = write(fd, &toSend, sizeof(toSend));
                // if (ret <= 0)
                //         fail("write");

                // ts.tv_sec = 30;
                // ts.tv_nsec = 0; /* 500 ms */
                // clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
                sleep(1);
                toSend = 0.12;
                printf("sending: %f\n", toSend);
                ret = write(fd, &toSend, sizeof(toSend));
                sleep(2);
                toSend = 0.22;
                printf("sending: %f\n", toSend);
                ret = write(fd, &toSend, sizeof(toSend));
                sleep(2);
                toSend = -0.12;
                printf("sending: %f\n", toSend);
                ret = write(fd, &toSend, sizeof(toSend));
                sleep(2);
                toSend = 0.0;
                printf("sending: %f\n", toSend);
                ret = write(fd, &toSend, sizeof(toSend));
                sleep(2);
        }
        return NULL;
}

void openport(int *port, int label)
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

        //openport(&fd_send, XDDP_PORT_SEND);
        openport(&fd_rec, XDDP_PORT_REC);

        //ret = pthread_create(&p_read, NULL, read_xddp, (void *)&fd_send);
        ret = pthread_create(&p_write, NULL, write_xddp, (void *)&fd_rec);
        pause();
        return 0;
}