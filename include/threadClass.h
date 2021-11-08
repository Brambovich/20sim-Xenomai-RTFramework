#ifndef THREADCLASS_H
#define THREADCLASS_H
#include <pthread.h>

class xenoThread_old
{
public:
    xenoThread_old(int prio);
    //setTimer();
    //initThread();
    int startThread(void *(*task)(void *), void *arg, const char *name);
    int setAffinity(int cpu);
    void stopThread();
    void joinThread();

private:
    bool isRunning;
    int priority;
    pthread_t thread;
    pthread_attr_t threadAttr;
    struct sched_param threadParam;
    void *task(void *arg);
};

#endif