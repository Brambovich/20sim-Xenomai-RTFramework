#include "multiCommClass.h"
#include <stdio.h>
#include <unistd.h>

IDDPComm::IDDPComm(int ownPort, int _sendSize, int _receiveSize)
{
    sendSize = _sendSize;
    receiveSize = _receiveSize;
    xenoCommClass = new IDDPconnection(ownPort);
    parameterSet = false;
}

IDDPComm::IDDPComm(int ownPort, int _size)
{
    sendSize = _size;
    receiveSize = _size;
    xenoCommClass = new IDDPconnection(ownPort);
    parameterSet = false;
}

IDDPComm::IDDPComm(int ownPort, int destPort, int _size, int _parameters[])
{
    sendSize = _size;
    receiveSize = _size;
    destinationPort = destPort;
    parameters = _parameters;
    xenoCommClass = new IDDPconnection(ownPort);
    parameterSet = true;
}
IDDPComm::~IDDPComm()
{
    xenoCommClass->~xenoCommunication();
}

XDDPComm::XDDPComm(int ownPort, int _sendSize, int _receiveSize)
{
    sendSize = _sendSize;
    receiveSize = _receiveSize;
    xenoCommClass = new XDDPconnection(ownPort);
}
XDDPComm::XDDPComm(int ownPort, int _size)
{
    sendSize = _size;
    receiveSize = _size;
    xenoCommClass = new XDDPconnection(ownPort);
}
XDDPComm::XDDPComm(int ownPort, int destPort, int _size, int _parameters[])
{
    sendSize = _size;
    receiveSize = _size;
    destinationPort = destPort;
    parameters = _parameters;
    xenoCommClass = new XDDPconnection(ownPort);
}
XDDPComm::~XDDPComm()
{
    xenoCommClass->~xenoCommunication();
}

void frameworkComm::setVerbose(bool _verbose)
{
    verbose = _verbose;
    xenoCommClass->setVerbose(verbose);
}

int frameworkComm::send(int destinationPort, double value[], int _parameters[])
{
    double toSend[sendSize];
    int ret;

    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[_parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = xenoCommClass->sendDoubleArray(toSend, destinationPort, sendSize);

    return ret;
}

int frameworkComm::send(double value[])
{
    //printf("send function of IDDP\n");
    double toSend[sendSize];
    int ret;
    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = xenoCommClass->sendDoubleArray(toSend, destinationPort, sendSize);
    return ret;
}

int frameworkComm::receive(double value[], int _parameters[])
{
    //printf("receive function of XDDP\n");
    double received[receiveSize];
    int ret;
    ret = xenoCommClass->receiveDoubleArray(received, receiveSize);
    for (int i = 0; i < receiveSize; i++)
    {
        value[_parameters[i]] = received[i];
    }

    return ret;
}
int frameworkComm::receive(double value[])
{
    double received[receiveSize];
    int ret;
    ret = xenoCommClass->receiveDoubleArray(received, receiveSize);
    if (ret > 0)
    {
        for (int i = 0; i < receiveSize; i++)
        {
            value[parameters[i]] = received[i];
        }
    }
    return ret;
}
