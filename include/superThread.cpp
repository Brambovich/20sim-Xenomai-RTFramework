#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "../include/gpioClass.h"
#include "runnableClass.h"
#include "superThread.h"
#include <signal.h>

xenoThread::xenoThread(runnable *_runClass)
{
    runClass = _runClass;
}
xenoThread::~xenoThread()
{
    if (timingLog)
    {
        loggingClass->~XDDPconnection();
    }
    printf("close XenoThread\n");
}
void xenoThread::init(int _ns_period, int _priority, int _affinity)
{
    ns_period = _ns_period;
    affinity = _affinity;
    priority = _priority;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(affinity, &cpuset);

    threadParam = {.sched_priority = priority};
    pthread_attr_init(&threadAttr);
    pthread_attr_setaffinity_np(&threadAttr, sizeof(cpuset), &cpuset);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
    pthread_attr_setschedparam(&threadAttr, &threadParam);
}

void xenoThread::init(int _ns_period, int _priority)
{
    ns_period = _ns_period;
    priority = _priority;

    threadParam = {.sched_priority = priority};
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
    pthread_attr_setschedparam(&threadAttr, &threadParam);
}

void xenoThread::inittimer(sigset_t *set, int freq_nanosecs)
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

    if (timer_create(CLOCK_MONOTONIC, &sevent, &timer1) == -1)
        perror("timer_create");

    new_value.it_interval.tv_sec = freq_nanosecs / 1000000000;
    new_value.it_interval.tv_nsec = freq_nanosecs % 1000000000;
    new_value.it_value.tv_sec = freq_nanosecs / 1000000000;
    new_value.it_value.tv_nsec = freq_nanosecs % 1000000000;

    if (timer_settime(timer1, 0, &new_value, &old_value) == -1)
        perror("timer_settime");
}
void xenoThread::enableLogging(bool _timingLog, int _loggingPort)
{
    loggingClass = new XDDPconnection(_loggingPort);
    loggingPort = _loggingPort;
    timingLog = _timingLog;
}

void xenoThread::enableGPIO(int pin)
{
    myGPIO = new xenoGPIO(pin);
    gpiobool = true;
    gpiopin = pin;
}

bool xenoThread::createThread()
{
    return (pthread_create(&thread, &threadAttr, InternalThreadEntryFunc, this) == 0);
}
int xenoThread::start(const char *name)
{
    int errno;

    createThread();
    errno = pthread_setname_np(thread, name);
    isRunning = true;
    return errno;
}
void xenoThread::toRun()
{

    int signum = SIGALRM;
    sigset_t set;
    int count = 0;
    inittimer(&set, ns_period);
    //runClass->setSize(2, 2); //WIP
    runClass->prerun();
    if (timingLog)
    {
        if (clock_gettime(CLOCK_MONOTONIC, &startTime) == -1)
            perror("clock_gettime");
    }
    if (sigwait(&set, &signum) == -1)
        perror("sigwait");
    while (1)
    {
        runClass->step();
        if (timingLog)
        {
            logTiming();
        }
        if (gpiobool)
        {
            myGPIO->toggle();
        }
        if (sigwait(&set, &signum) == -1)
            perror("sigwait");
    }
}
int xenoThread::setAffinity()
{
    cpu_set_t cpuset;
    int ret;
    CPU_ZERO(&cpuset);

    CPU_SET(affinity, &cpuset);
    ret = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    return ret;
}

void xenoThread::stopThread()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    runClass->postrun();
    isRunning = false;
    return;
}

void xenoThread::logTiming()
{
    if (clock_gettime(CLOCK_MONOTONIC, &nowTime) == -1)
        perror("clock_gettime");
    double toSend = 0.00;

    if (nowTime.tv_nsec - startTime.tv_nsec < 0)
    {
        toSend = toSend + (double)(nowTime.tv_sec - startTime.tv_sec - 1);
        toSend = toSend + ((double)(1000000000 + (nowTime.tv_nsec - startTime.tv_nsec))) / 1000000000;
        printf(" --- %lds %ldns\n", (nowTime.tv_sec - startTime.tv_sec - 1), (1000000000 + (nowTime.tv_nsec - startTime.tv_nsec)));
    }
    else
    {
        toSend = toSend + (double)(nowTime.tv_sec - startTime.tv_sec);
        toSend = toSend + ((double)(nowTime.tv_nsec - startTime.tv_nsec)) / 1000000000;
        printf(" --- %lds %ldns ---\n", (nowTime.tv_sec - startTime.tv_sec), (nowTime.tv_nsec - startTime.tv_nsec));
    }

    loggingClass->sendDouble(toSend, loggingPort);
}
