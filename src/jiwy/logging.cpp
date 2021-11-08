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

bool timing = true;

static void fail(const char *reason)
{
    perror(reason);
    exit(EXIT_FAILURE);
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
    //int numberOfLogs = argv[0];
    //
    printf("start logging...\n");
    int numberOfLogs;
    if (timing)
    {
        numberOfLogs = 2;
    }
    else
    {
        numberOfLogs = 4;
    }
    double received[numberOfLogs];
    int fd[numberOfLogs];
    for (int i = 0; i < numberOfLogs; i++)
    {
        openport(&fd[i], (25 + i));
        printf("opened port %d\n", i);
    }
    int ret;
    std::ofstream myfile;
    myfile.open("jiwy_timing.csv");
    if (timing)
    {
        myfile << "PlantTime,"
               << "ControllerTime"
               << "\n";
    }
    else
    {
        myfile << "ControllerTime,"
               << "Controller,"
               << "PlantTime,"
               << "Plant"
               << "\n";
    }
    for (;;)
    {
        /* Get the next message from realtime_thread2. */
        for (int i = 0; i < numberOfLogs; i++)
        {
            ret = read(fd[i], &received[i], sizeof(*received));
            if (ret <= 0)
                fail("read");
            printf("received[%d] = %.15g\n", i, received[i]);
            myfile << std::setprecision(std::numeric_limits<double>::digits10 + 1) << received[i];
            if (i < numberOfLogs - 1)
            {
                myfile << ",";
            }
        }
        if (received[1] > 10)
        {
            break;
        }
        myfile << "\n";
    }
    myfile.close();
    return 1;
}