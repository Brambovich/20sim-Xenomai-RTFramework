// Header file for input output functions
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <rtdm/ipc.h>
#include <sys/syscall.h>
#include <sched.h>

#include "../Plant/Plant.h"
#include "../Controller/Controller.h"
#include "../include/threadClass.h"
#include "../include/communicationClass.h"

#define PERIOD_SECS 1     //10millisecs
#define PERIOD_NANOSECS 0 //200000000

#define XDDP 1
#define IDDP 2
// main function -
// where the execution of program begins
timer_t timer1;

bool verbose = true;

void inittimer(sigset_t *set)
{
    struct itimerspec new_value, old_value;
    struct sigaction action;
    struct sigevent sevent;

    int signum = SIGALRM;

    sevent.sigev_notify = SIGEV_THREAD_ID;
    sevent.sigev_notify_thread_id = syscall(__NR_gettid);
    sevent.sigev_signo = signum;

    sigemptyset(set);
    sigaddset(set, signum);
    sigprocmask(SIG_BLOCK, set, NULL);

    // if (timer_create(CLOCK_MONOTONIC, &sevent, &timer1) == -1)
    //     perror("timer_create");
    if (timer_create(CLOCK_MONOTONIC, 0, &timer1) == -1)
        perror("timer_create\n");

    new_value.it_interval.tv_sec = PERIOD_SECS;
    new_value.it_interval.tv_nsec = PERIOD_NANOSECS;
    new_value.it_value.tv_sec = PERIOD_SECS;
    new_value.it_value.tv_nsec = PERIOD_NANOSECS;

    if (timer_settime(timer1, 0, &new_value, &old_value) == -1)
        perror("timer_settime\n");
}

static void *Controller_task(void *arg)
{
    XDDPconnection control_iddp(12);
    sigset_t set;
    int signum = SIGALRM;
    int ret;
    inittimer(&set);
    int size = 1;
    double count = 1;
    int portnumber = 2;
    while (1)
    {
        ret = control_iddp.sendDouble(count, portnumber);
        sigwait(&set, &signum);
    }
    return NULL;
}

static void *Plant_task(void *arg)
{
    XDDPconnection plant_iddp(3);
    sigset_t set;
    int signum = SIGALRM;
    int ret = 1;
    inittimer(&set);
    int count = 0;
    int size = 1;
    double received;

    while (1)
    {
        ret = plant_iddp.receiveRecentDouble(&received);
        sigwait(&set, &signum);
    }

    return NULL;
}

int main()
{
    // runnable *example = new ownThread(true);
    // xenoThreadz testclass(example);

    // sim20 *togive = new sim20;
    // runnable *example2 = new wrapper(togive);
    // xenoThreadz testclass2(example2);

    // example->testfunction(5);

    xenoThread_old plantThread(80);
    xenoThread_old controllerThread(79);

    plantThread.startThread(&Plant_task, NULL, "Plant");
    controllerThread.startThread(&Controller_task, NULL, "Controller");

    plantThread.setAffinity(1);
    controllerThread.setAffinity(1);

    pause();

    plantThread.stopThread();
    controllerThread.stopThread();
    return 0;
}