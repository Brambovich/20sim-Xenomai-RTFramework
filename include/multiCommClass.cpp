#include "multiCommClass.h"
#include <stdio.h>
#include <unistd.h>

IDDPComm::IDDPComm(int ownPort, int _sendSize, int _receiveSize)
{
    sendSize = _sendSize;
    receiveSize = _receiveSize;
    iddpclass = new IDDPconnection(ownPort);
    parameterSet = false;
}

IDDPComm::IDDPComm(int ownPort, int _size)
{
    sendSize = _size;
    receiveSize = _size;
    iddpclass = new IDDPconnection(ownPort);
    parameterSet = false;
}

IDDPComm::IDDPComm(int ownPort, int destPort, int _size, int _parameters[])
{
    sendSize = _size;
    receiveSize = _size;
    destinationPort = destPort;
    parameters = _parameters;
    iddpclass = new IDDPconnection(ownPort);
    parameterSet = true;
}
IDDPComm::~IDDPComm()
{
    iddpclass->~IDDPconnection();
}
int IDDPComm::send(int destinationPort, double value[], int _parameters[])
{
    double toSend[sendSize];
    int ret;

    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[_parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = iddpclass->sendDoubleArray(toSend, destinationPort, sendSize);

    return ret;
}

int IDDPComm::send(double value[])
{
    //printf("send function of IDDP\n");
    double toSend[sendSize];
    int ret;
    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = iddpclass->sendDoubleArray(toSend, destinationPort, sendSize);
    return ret;
}

int IDDPComm::receive(double value[], int parameters[])
{
    //printf("send function of IDDP\n");
    double received[receiveSize];
    int ret;
    ret = iddpclass->receiveDoubleArray(received, receiveSize);
    for (int i = 0; i < receiveSize; i++)
    {
        value[parameters[i]] = received[i];
    }

    return ret;
}

int IDDPComm::receive(double value[])
{
    double received[receiveSize];
    int ret;

    ret = iddpclass->receiveDoubleArray(received, receiveSize);

    if (ret > 0)
    {
        for (int i = 0; i < receiveSize; i++)
        {
            value[parameters[i]] = received[i];
        }
    }
    return ret;
}

XDDPComm::XDDPComm(int ownPort, int _sendSize, int _receiveSize)
{
    sendSize = _sendSize;
    receiveSize = _receiveSize;
    xddpclass = new XDDPconnection(ownPort);
}
XDDPComm::XDDPComm(int ownPort, int _size)
{
    sendSize = _size;
    receiveSize = _size;
    xddpclass = new XDDPconnection(ownPort);
}
XDDPComm::XDDPComm(int ownPort, int destPort, int _size, int _parameters[])
{
    sendSize = _size;
    receiveSize = _size;
    destinationPort = destPort;
    parameters = _parameters;
    xddpclass = new XDDPconnection(ownPort);
}
XDDPComm::~XDDPComm()
{
    xddpclass->~XDDPconnection();
}
int XDDPComm::send(int destinationPort, double value[], int _parameters[])
{
    double toSend[sendSize];
    int ret;
    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[_parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = xddpclass->sendDoubleArray(toSend, 5, sendSize);

    return ret;
}
int XDDPComm::send(double value[])
{
    //printf("send function of XDDP\n");
    double toSend[sendSize];
    int ret;
    for (int i = 0; i < sendSize; i++)
    {
        toSend[i] = value[parameters[i]];
        //printf("value on %d is: %f\n", parameters[i], value[parameters[i]]);
    }
    ret = xddpclass->sendDoubleArray(toSend, destinationPort, sendSize);
    return ret;
}
int XDDPComm::receive(double value[], int _parameters[])
{
    //printf("receive function of XDDP\n");
    double received[receiveSize];
    int ret;
    ret = xddpclass->receiveDoubleArray(received, receiveSize);
    for (int i = 0; i < receiveSize; i++)
    {
        value[_parameters[i]] = received[i];
    }

    return ret;
}
int XDDPComm::receive(double value[])
{
    double received[receiveSize];
    int ret;
    ret = xddpclass->receiveDoubleArray(received, receiveSize);
    if (ret > 0)
    {
        for (int i = 0; i < receiveSize; i++)
        {
            value[parameters[i]] = received[i];
        }
    }
    return ret;
}