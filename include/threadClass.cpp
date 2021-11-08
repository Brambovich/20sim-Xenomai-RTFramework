#include "threadClass.h"
#include <pthread.h>
#include <stdio.h>

xenoThread_old::xenoThread_old(int _priority)
{
    priority = _priority;
    threadParam = {.sched_priority = priority};
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
    pthread_attr_setschedparam(&threadAttr, &threadParam);
}

int xenoThread_old::startThread(void *(*task)(void *), void *arg, const char *name)
{
    int errno;

    errno = pthread_create(&thread, &threadAttr, task, arg);
    errno = pthread_setname_np(thread, name);
    isRunning = true;
    return errno;
}

void xenoThread_old::stopThread()
{
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    isRunning = false;
    return;
}

void xenoThread_old::joinThread()
{
    pthread_join(thread, NULL);
    isRunning = false;
    return;
}

int xenoThread_old::setAffinity(int cpu)
{
    cpu_set_t cpuset;
    int ret;
    CPU_ZERO(&cpuset);

    CPU_SET(cpu, &cpuset);
    ret = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    return ret;
}