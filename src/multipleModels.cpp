#include <stdio.h>
#include <unistd.h>
#include <iterator>
#include <signal.h>
#include <vector>
#include <sys/syscall.h>
#include <array>
#include <string>

#include "../Submodel1/Submodel1.h"

#include "../include/multiCommClass.h"
#include "../include/runnableClass.h"
#include "../include/superThread.h"

bool exitbool = true;

#define XDDP_PORT_SEND 3
#define XDDP_PORT_REC 2

void printtime(struct timespec *start)
{
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
        perror("clock_gettime");
    if (tp.tv_nsec - start->tv_nsec < 0)
    {
        printf(" --- %lds %ldns\n", (tp.tv_sec - start->tv_sec - 1), (1000000000 + (tp.tv_nsec - start->tv_nsec)));
    }
    else
    {
        printf(" --- %lds %ldns ---\n", (tp.tv_sec - start->tv_sec), (tp.tv_nsec - start->tv_nsec));
    }
}

void openport(int *port, int label)
{
    char buf[128], *devname;
    int ret, n = 0;
    if (asprintf(&devname, "/dev/rtp%d", label) < 0)
        printf("asprintf");
    *port = open(devname, O_NONBLOCK | O_RDWR);
    free(devname);
    if (port < 0)
        perror("open");
}

int main()
{
    struct timespec start;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        perror("clock_gettime");
    //CREATE PARAM AND WRAPPER FOR CONTROLLER
    for (int modelCount = 2; modelCount < 10; modelCount++)
    {

        int uPortsSize = 1;
        int yPortsSize = 1;
        int Submodel1_yParam[] = {0};
        int Submodel1_uParam[] = {0};
        std::vector<runnable *> runners;
        {
            frameworkComm **Submodel1_uPorts;
            Submodel1_uPorts = (frameworkComm **)calloc(uPortsSize, sizeof(frameworkComm *));
            Submodel1_uPorts[0] = new XDDPComm(1, -1, 1, Submodel1_uParam);

            frameworkComm **Submodel1_yPorts;
            Submodel1_yPorts = (frameworkComm **)calloc(yPortsSize, sizeof(frameworkComm *));
            Submodel1_yPorts[0] = new IDDPComm(2, 3, 1, Submodel1_yParam);

            Submodel1 *Submodel1_class = new Submodel1;
            runners.push_back(new wrapper<Submodel1>(Submodel1_class, Submodel1_uPorts, Submodel1_yPorts, uPortsSize, yPortsSize));
        }

        if (modelCount > 2)
        {
            for (int numberOfModel = 0; numberOfModel < (modelCount - 2); numberOfModel++)
            {
                frameworkComm **Submodel1_uPorts;
                Submodel1_uPorts = (frameworkComm **)calloc(uPortsSize, sizeof(frameworkComm *));
                Submodel1_uPorts[0] = new IDDPComm((numberOfModel * 2) + 3, -1, 1, Submodel1_uParam);

                frameworkComm **Submodel1_yPorts;
                Submodel1_yPorts = (frameworkComm **)calloc(yPortsSize, sizeof(frameworkComm *));
                Submodel1_yPorts[0] = new IDDPComm((numberOfModel * 2) + 4, (numberOfModel * 2) + 5, 1, Submodel1_yParam);
                Submodel1 *Submodel1_class = new Submodel1;
                runners.push_back(new wrapper<Submodel1>(Submodel1_class, Submodel1_uPorts, Submodel1_yPorts, uPortsSize, yPortsSize));
            }
        }

        if (modelCount > 1)
        {

            frameworkComm **Submodel1_uPorts;
            Submodel1_uPorts = (frameworkComm **)calloc(uPortsSize, sizeof(frameworkComm *));
            Submodel1_uPorts[0] = new IDDPComm((modelCount * 2) - 1, -1, 1, Submodel1_uParam);

            frameworkComm **Submodel1_yPorts;
            Submodel1_yPorts = (frameworkComm **)calloc(yPortsSize, sizeof(frameworkComm *));
            Submodel1_yPorts[0] = new XDDPComm((modelCount * 2), (modelCount * 2), 1, Submodel1_yParam);

            Submodel1 *Submodel1_class = new Submodel1;
            runners.push_back(new wrapper<Submodel1>(Submodel1_class, Submodel1_uPorts, Submodel1_yPorts, uPortsSize, yPortsSize));
        }

        //INIT XENOTRHEAD FOR CONTROLLER + PLANT AND START
        std::vector<xenoThread *> threads;
        for (int j = 0; j < modelCount; j++)
        {
            runners[j]->setVerbose(false);
            runners[j]->setSize(1, 1);
            threads.push_back(new xenoThread(runners[j]));
            threads[j]->init(1000000, (69 + j), 1);
        }
        for (int j = 0; j < modelCount; j++)
        {
            threads[j]->start("submodel"); //(const char *)j);
        }

        //  WAIT  /////////////////////////////////////////////////////////////
        printf("all classes created, start wait period...\n");
        int fd_send, fd_rec, ret;
        openport(&fd_send, (modelCount * 2));
        openport(&fd_rec, 1);
        double toSend = 0.777;
        double received = 0;
        for (int i = 0; i < 5; i++)
        {
            printtime(&start);
            while (ret > 0)
            {
                ret = read(fd_send, &received, sizeof(received));
            }
            printf("\n");
            printf(" --- NRT --- %d: received: %f\n", ret, received);

            ret = write(fd_rec, &toSend, sizeof(toSend));
            printf(" --- NRT --- %d: sending: %f\n", ret, toSend);

            usleep(1000000);
        }

        close(fd_send);
        close(fd_rec);
        //  CLEANUP  //////////////////////////////////////////////////////////
        for (int j = 0; j < modelCount; j++)
        {
            threads[j]->stopThread();
            runners[j]->~runnable();
        }
        usleep(100000);
    }

    return 1;
}