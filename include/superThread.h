#ifndef SUPERTHREADCLASS_H
#define SUPERTHREADCLASS_H

#include "communicationClass.h"
#include "gpioClass.h"
/**
* Class that creates and runs task on a specified timer. Is able to log timing and toggle GPIO for timing analysis. 
* @brief Thread creation class
*/
class xenoThread
{
public:
    xenoThread(runnable *_runClass);
    ~xenoThread();
    /**
     * @brief initalization function to set the timer, priority and core affinity
     * @param _ns_period task period in ns
     * @param _priority task scheduling priority
     * @param _affinity task affinity
     * @see init(int _ns_period, int _priority);
     */
    void init(int _ns_period, int _priority, int _affinity);
    /**
     * @brief initalization function to set the timer and priority.
     * @param _ns_period task period in ns
     * @param _priority task scheduling priority
     * @see void init(int _ns_period, int _priority, int _affinity);
     */
    void init(int _ns_period, int _priority);
    /**
     * @brief enables timing logging to an XDDP port
     * 
     * @param _timingLog enable or disable timing logging
     * @param _loggingPort logging port for the XDDP comm
     */
    void enableLogging(bool _timingLog, int _loggingPort);
    /**
     * @brief Start the xenoThread thread function. 
     * 
     * @param name Name of the thread visible in xenomai thread stat. 
     * @return int: 1 if correctly started. 
     */
    int start(const char *name);
    /**
     * @brief (TODO) Set the Affinity of the thread. 
     * 
     * @return int: returns the core on which the thread is set.
     */
    int setAffinity();
    /**
     * @brief enables GPIO timing logging of the task. 
     * 
     * @param pin Pin which is toggled every period. 
     */
    void enableGPIO(int pin);
    /**
     * @brief Stop the running thread. 
     * 
     */
    void stopThread();

private:
    /**
     * @brief Internal function: logs timing information to set XDDP port. 
     * 
     */
    void logTiming();
    /**
     * @brief Internal function that calls an internal function. 
     * Function that receives its own class to run the toRun() function. Declarated inside of the .h such that xenoThread is declarated. 
     * @param This must be 'this'. (own instance)
     */
    static void *InternalThreadEntryFunc(void *This)
    {
        ((xenoThread *)This)->toRun();
        return NULL;
    }
    /**
     * @brief Internal function: Starts the required timer and creates signals
     * Timer is awaited using sigwait(&set, &signum). 
     * @param set any set created with sigset_t set;
     * @param freq_nanosecs frequency in ns. 
     */
    void inittimer(sigset_t *set, int freq_nanosecs);
    /**
     * @brief Internal function called by 'internalThreadEntryFunc' that contains thread functionality. 
     * 
     */
    void toRun();
    /**
     * @brief Create a Thread with pthread create. When using xenomai posix skin the thread is started in real-time. 
     * 
     * @return 
     */
    bool createThread();
    pthread_t thread;
    timer_t timer1;
    pthread_attr_t threadAttr;
    bool gpiobool = false;
    int gpiopin;
    xenoGPIO *myGPIO;
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