#ifndef SUPERTHREADCLASS_H
#define SUPERTHREADCLASS_H

#include "communicationClass.h"

class xenoThread
{
public:
    xenoThread(runnable *_runClass);
    ~xenoThread();
    void init(int _ns_period, int _priority, int _affinity);
    void init(int _ns_period, int _priority);
    void inittimer(sigset_t *set, int freq_nanosecs);
    bool createThread();
    int start(const char *name);
    void enableLogging(bool _timingLog, int _loggingPort);
    void logTiming();
    void toRun();
    int setAffinity();
    void stopThread();
    static void *InternalThreadEntryFunc(void *This)
    {
        ((xenoThread *)This)->toRun();
        return NULL;
    }

private:
    pthread_t thread;
    timer_t timer1;
    pthread_attr_t threadAttr;
    struct sched_param threadParam;
    runnable *runClass;
    bool isRunning;
    int priority;
    bool timingLog = false;
    int loggingPort;
    int affinity = -1;
    int ns_period;
    struct timespec startTime, nowTime;
    XDDPconnection *loggingClass;
};

#endif