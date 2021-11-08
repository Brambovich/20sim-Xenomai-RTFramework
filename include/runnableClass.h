#ifndef RUNNABLECLASS_H
#define RUNNABLECLASS_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "multiCommClass.h"
#include "../include/gpioClass.h"

class runnable
{
public:
    virtual ~runnable() { printf("close Runnable\n"); }
    virtual void prerun() = 0;
    virtual void postrun() = 0;
    virtual void run() = 0;
    virtual void step() = 0;
    virtual void setSize(int uSize, int ySize) = 0;
    virtual void enableGPIO(int pin) = 0;
    virtual void setVerbose(bool verbose) = 0;
};

template <typename T>
class wrapper : public runnable
{
public:
    /**
     * @brief Construct a new wrapper object
     * Init for the wrapper object which is a derivative of the virtual runnable class. This class will execute a 20-sim model.
     * @param _20simclass The 20-sim object which needs to be run, is initialized in compiler time with a template.
     * @param _receiveClass An array containing frameworkComm classes to receive the value before calculate()
     * @param _sendClass An array containing frameworkComm classes to receive the value after calculate() 
     * @param _receiveArraySize The size of the receiveClass array
     * @param _sendArraySize The size of the sendClass array
     */
    wrapper(T *_20simclass, frameworkComm *_receiveClass[], frameworkComm *_sendClass[], int _receiveArraySize, int _sendArraySize)
    {
        receiveArraySize = _receiveArraySize;
        sendArraySize = _sendArraySize;
        receiveClass = _receiveClass;
        sendClass = _sendClass;
        simclass20 = _20simclass;
    }
    ~wrapper() override
    {
        printf("close wrapper and free memory...\n");
        free(u);
        free(y);
        for (int i = 0; i < receiveArraySize; i++)
        {
            receiveClass[i]->~frameworkComm();
        }
        for (int i = 0; i < sendArraySize; i++)
        {
            sendClass[i]->~frameworkComm();
        }
    }
    /**
     * @brief sets Sizes.
     * Set the sizes of the begin and end values and create their respective arrays.
     * @param _uSize sets the size for the receiving (begin) value.
     * @param _ySize sets the size for the sending (end) value.
     */
    void setSize(int _uSize, int _ySize)
    {
        ySize = _ySize;
        uSize = _uSize;
        u = (double *)calloc(uSize, sizeof(double));
        y = (double *)calloc(ySize, sizeof(double));
        sizesSet = true;
    }
    void setVerbose(bool _verbose)
    {
        verbose = _verbose;
        for (int i = 0; i < sendArraySize; i++)
        {
            sendClass[i]->setVerbose(verbose);
        }
        for (int i = 0; i < receiveArraySize; i++)
        {
            receiveClass[i]->setVerbose(verbose);
        }
    }
    /**
     * @brief Initialization function
     * Prerun function containing all prerun functionality and initialization of the 20-sim-submodel.
     * 
     */
    void prerun() override
    {
        if (!sizesSet)
        {
            perror("FAIL : sizes not set, run setSize(x,y) before start\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < uSize; i++)
        {
            u[i] = 0.0;
        }
        for (int i = 0; i < ySize; i++)
        {
            y[i] = 0.0;
        }

        simclass20->Initialize(u, y, 0.0);
    }
    /**
     * @brief Single run step function
     * Function which calculates the submodel for 1 timestep.
     * 
     */
    void step() override
    {
        // send all the values from the set send class array.
        for (int i = 0; i < receiveArraySize; i++)
        {
            receiveClass[i]->receive(u);
        }
        if (verbose)
        {
            for (int i_u = 0; i_u < uSize; i_u++)
            {
                printf(" --- u[%d] = %f", i_u, u[i_u]);
            }
            printf("\n");
        }
        //printf(" 2 u[1] = %f\n", u[1]);
        simclass20->Calculate(u, y);
        if (verbose)
        {
            for (int i_y = 0; i_y < ySize; i_y++)
            {
                printf(" --- y[%d] = %f", i_y, y[i_y]);
            }
            printf("\n");
        }
        //receive all the values from the set receive class array
        for (int i = 0; i < sendArraySize; i++)
        {
            sendClass[i]
                ->send(y);
        }
        if (gpiobool)
        {
            myGPIO->toggle();
        }
    }

    void enableGPIO(int pin) override
    {
        myGPIO = new xenoGPIO(pin);
        gpiobool = true;
        gpiopin = pin;
    }
    void run() override {}
    void postrun() override {}

private:
    bool gpiobool = false;
    bool sizesSet = false;
    bool verbose = false;
    int gpiopin;
    T *simclass20;
    xenoGPIO *myGPIO;
    int uSize;
    int ySize;
    int sendArraySize;
    int receiveArraySize;
    frameworkComm **receiveClass;
    frameworkComm **sendClass;
    double *u;
    double *y;
};

#endif