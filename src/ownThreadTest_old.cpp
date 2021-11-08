#include <stdio.h>
#include <unistd.h>
#include <iterator>
#include <signal.h>
#include <vector>
#include <sys/syscall.h>
#include <array>
#include <string>
#include <math.h>
#include <fstream>
#include <iomanip>
#include <limits>
#include "../include/multiCommClass.h"
#include "../include/runnableClass.h"
#include "../include/superThread.h"

bool exitbool = true;

void my_handler(int s)
{
    printf("Caught signal %d\n", s);
    exitbool = false;
}

class logger : public runnable
{
public:
    logger(int _noPrimes)
    {
        noPrimes = _noPrimes;
    }
    void step() override
    {
        int primes = 0;
        for (int i = 2; i < noPrimes; i++)
        {
            for (int j = 2; j * j <= i; j++)
            {
                if (i % j == 0)
                    break;
                else if (j + 1 > sqrt(i))
                {
                    primes++;
                }
            }
        }
        if (verbose)
        {
            printf("primes = %d\n", primes);
            printf("step: %d\n", count);
        }
        count++;
    }
    void prerun() override
    {
        printf("prerun...\n");
    }
    void postrun() override
    {
    }
    void run() override {}
    void setSize(int uSize, int ySize) override {}
    void enableGPIO(int pin) override {}
    void setVerbose(bool _verbose) { verbose = _verbose; }

private:
    bool verbose = false;
    int count = 0;
    int noPrimes;
};

class ownThread : public runnable
{
public:
    ownThread(int _noPrimes)
    {
        noPrimes = _noPrimes;
    }
    void step() override
    {
        int primes = 0;
        for (int i = 2; i < noPrimes; i++)
        {
            for (int j = 2; j * j <= i; j++)
            {
                if (i % j == 0)
                    break;
                else if (j + 1 > sqrt(i))
                {
                    primes++;
                }
            }
        }
        if (verbose)
        {
            printf("primes = %d\n", primes);
            printf("step: %d\n", count);
        }
        count++;
    }
    void prerun() override
    {
        printf("prerun...\n");
    }
    void postrun() override {}
    void run() override {}
    void setSize(int uSize, int ySize) override {}
    void enableGPIO(int pin) override {}
    void setVerbose(bool _verbose) { verbose = _verbose; }

private:
    bool verbose = false;
    int count = 0;
    int noPrimes;
};

void openport(int *port, int label)
{
    char buf[128], *devname;
    int ret, n = 0;
    if (asprintf(&devname, "/dev/rtp%d", label) < 0)
        printf("asprintf");
    *port = open(devname, O_RDWR);
    free(devname);
    if (port < 0)
        perror("open");
}

int main()
{
    for (int modelCount = 1; modelCount < 4; modelCount++)
    {
        int prime = 300;
        // // for (int prime = 200; prime < 1000; prime += 200)
        // // {
        // int modelCount = 2;

        std::vector<runnable *> runners;
        for (int i = 0; i < modelCount; i++)
        {
            runners.push_back(new ownThread(prime));
        }
        std::vector<xenoThread *> threads;

        for (int i = 0; i < modelCount; i++)
        {
            printf("setup...\n");
            runners[i]->setVerbose(false);
            threads.push_back(new xenoThread(runners[i]));
            threads[i]->init(1000000, (95), 0 + i);
            threads[i]->enableLogging(true, 20 + i);
            // threads[i]->enableGPIO(22 + i);
        }
        printf("finished...\n");
        for (int i = 0; i < modelCount; i++)
        {
            printf("start...\n");
            threads[i]->start((const char *)i);
        }
        //WAITING////////////////

        // printf("start waiting");
        // sleep(15);
        // double received[modelCount];
        // int fd[modelCount];
        // for (int i = 0; i < modelCount; i++)
        // {
        //     openport(&fd[i], (20 + i));
        // }
        // int ret;
        // std::ofstream myfile;
        // std::string endname = ".csv";

        // std::string name = "primes";
        // myfile.open(name + std::to_string(prime) + endname);
        // for (int i = 0; i < modelCount; i++)
        // {
        //     myfile << "thread" << i;
        //     if (i < modelCount - 1)
        //     {
        //         myfile << ",";
        //     }
        // }
        // myfile << "\n";
        // // myfile << "thread1,thread2"
        // //        << "\n";
        // printf("START RECORDING FOR %D MODELS", modelCount);
        // for (;;)
        // {

        //     /* Get the next message from realtime_thread2. */
        //     for (int i = 0; i < modelCount; i++)
        //     {
        //         ret = read(fd[i], &received[i], sizeof(*received));
        //         if (ret <= 0)
        //             perror("read");
        //     }
        //     for (int i = 0; i < modelCount; i++)
        //     {
        //         //printf("send to file: %f", received[i]);
        //         myfile << std::setprecision(std::numeric_limits<double>::digits10 + 1) << received[i];
        //         if (i < modelCount - 1)
        //         {
        //             myfile << ",";
        //         }
        //     }
        //     printf("received[0] = %f\n", received[0]);
        //     myfile << "\n";
        //     if (received[0] > 5)
        //     {
        //         printf("\n\n BREAK! \n\n");
        //         break;
        //     }
        // }
        // myfile.close();
        // for (int i = 0; i < modelCount; i++)
        // {
        //     close(fd[i]);
        // }
        // //ENDING////////////////
        for (int j = 0; j < modelCount; j++)
        {
            threads[j]->stopThread();
            threads[j]->~xenoThread();
            runners[j]->~runnable();
        }
        sleep(1);
    }
    // runnable *plant_runnable = new ownThread(40000);
    // runnable *controller_runnable = new ownThread(10000);

    // xenoThread plantClass(plant_runnable);
    // xenoThread controllerClass(controller_runnable);
    // plantClass.init(100000000, 80, 1);
    // controllerClass.init(100000000, 79, 1);

    // // controllerClass.enableLogging(true, 25);
    // // plantClass.enableLogging(true, 26);
    // //START THREADS
    // controllerClass.start("controller");
    // plantClass.start("plant");

    // // WAIT FOR CNTRL-C
    // while (exitbool)
    //     ;
    // printf("STOP RUNNNING...\n");

    // //CLEANUP
    // controllerClass.stopThread();
    // plantClass.stopThread();
    // controller_runnable->~runnable();
    // plant_runnable->~runnable();
    return 1;
}