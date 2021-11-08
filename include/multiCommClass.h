#ifndef MULTICOMMCLASS_H
#define MULTICOMMCLASS_H

#include "communicationClass.h"

class frameworkComm
{
public:
    virtual int send(int destinationPort, double value[], int parameters[]) = 0;
    virtual int send(double value[]) = 0;
    virtual int receive(double value[], int parameters[]) = 0;
    virtual int receive(double value[]) = 0;
    virtual ~frameworkComm() {}
};

class IDDPComm : public frameworkComm
{
public:
    IDDPComm(int ownPort, int _sendSize, int _receiveSize);
    IDDPComm(int ownPort, int _size);
    IDDPComm(int ownPort, int destPort, int _size, int _parameters[]);
    ~IDDPComm();
    int send(int destinationPort, double value[], int _parameters[]);
    int send(double value[]);
    int receive(double value[], int parameters[]);
    int receive(double value[]);

private:
    bool parameterSet;
    int *parameters;
    int destinationPort;
    int receiveSize;
    int sendSize;
    IDDPconnection *iddpclass;
};

class XDDPComm : public frameworkComm
{
public:
    XDDPComm(int ownPort, int _sendSize, int _receiveSize);
    XDDPComm(int ownPort, int _size);
    XDDPComm(int ownPort, int destPort, int _size, int _parameters[]);
    ~XDDPComm();
    int send(int destinationPort, double value[], int _parameters[]);
    int send(double value[]);
    int receive(double value[], int _parameters[]);
    int receive(double value[]);

private:
    int *parameters;
    int receiveSize;
    int sendSize;
    int destinationPort;
    XDDPconnection *xddpclass;
};

#endif