#include "communicationClass.h"
#include <rtdm/ipc.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

void IDDPconnection::initSocket()
{
    protocol = "IDDP";
    int ret;
    size_t poolsz;
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_IDDP);
    poolsz = 32768; /* bytes */
    ret = setsockopt(s, SOL_IDDP, IDDP_POOLSZ,
                     &poolsz, sizeof(poolsz));
    // ret = setsockopt(s, SOL_IDDP, IDDP_POOLSZ,
    //                  NULL, 0);
    if (ret)
        perror("setsockopt");
    memset(&addr, 0, sizeof(addr));
    addr.sipc_family = AF_RTIPC;
    addr.sipc_port = receivePort;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret)
        perror("setsockopt");
    printf("Done setting up the IDDP for port:%d - return: %d\n", receivePort, ret);
    return;
}

void XDDPconnection::initSocket()
{
    protocol = "XDDP";
    int ret;
    size_t poolsz;
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    poolsz = 32768; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ,
                     &poolsz, sizeof(poolsz));
    if (ret)
        perror("setsockopt");

    memset(&addr, 0, sizeof(addr));
    addr.sipc_family = AF_RTIPC;
    addr.sipc_port = receivePort;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret)
        perror("setsockopt");
    printf("Done setting up the XDDP for port:%d - return: %d\n", receivePort, ret);
    return;
}

XDDPconnection::XDDPconnection(int r_recP)
{
    receivePort = r_recP;
    initSocket();
}
XDDPconnection::~XDDPconnection()
{
    printf("Close XDDP connection and close port: %d\n", receivePort);
    close(s);
}

IDDPconnection::IDDPconnection(int r_recP)
{
    receivePort = r_recP;
    initSocket();
}

IDDPconnection::~IDDPconnection()
{
    printf("Close IDDP connection and close port: %d\n", receivePort);
    close(s);
}

void xenoCommunication::setVerbose(bool _verbose)
{
    verbose = _verbose;
}

int xenoCommunication::sendDouble(double toSend, int port)
{
    //printf("inside sending\n");
    struct sockaddr_ipc sendAddr;
    int ret;
    sendAddr.sipc_family = AF_RTIPC;
    sendAddr.sipc_port = port;
    ret = sendto(s, &toSend, sizeof(toSend), MSG_DONTWAIT,
                 (struct sockaddr *)&sendAddr, sizeof(sendAddr));

    if (verbose)
    {
        printf("%s - sent %d bytes, \"%f\" to port: %d\n", protocol, ret, toSend, port);
    }
    return ret;
}

int xenoCommunication::sendDoubleArray(double toSend[], int port, int size)
{
    //printf("inside sending\n");
    struct sockaddr_ipc sendAddr;
    int ret;
    sendAddr.sipc_family = AF_RTIPC;
    sendAddr.sipc_port = port;
    ret = sendto(s, toSend, (size * sizeof(*toSend)), MSG_DONTWAIT,
                 (struct sockaddr *)&sendAddr, sizeof(sendAddr));
    if (verbose)
    {
        printf("%s - sent %d bytes:  ", protocol, (size * sizeof(*toSend)));
        for (int i = 0; i < size; i++)
        {
            printf("%f", toSend[i]);
        }
        printf("  to port: %d from port %d - return: %d\n", port, receivePort, ret);
    }
    return ret;
}

int xenoCommunication::sendDouble(double toSend)
{
    //printf("inside sending\n");
    int ret;
    ret = sendto(s, &toSend, sizeof(toSend), MSG_DONTWAIT, NULL, 0);

    if (verbose)
    {
        printf("%s - sent %d bytes, \"%f\" to port: %d\n", protocol, ret, toSend, receivePort);
    }
    return ret;
}

int xenoCommunication::receiveRecentDouble(double *received)
{

    //printf("inside receiving\n");
    struct sockaddr_ipc recAddr;
    int ret = 1;
    int ret_count = 0;
    //double received;
    socklen_t addrlen;
    addrlen = sizeof(addr);
    while (ret > 0)
    {
        //ret = recvfrom(s, received, sizeof(*received), MSG_DONTWAIT, (struct sockaddr *)&recAddr, &addrlen);
        ret = recvfrom(s, received, sizeof(*received), MSG_DONTWAIT, NULL, 0);
        if (ret > 0)
        {
            ret_count++;
        }
    }
    if (verbose)
    {
        printf("%s - received %d values, most recent: \"%f\" from port %d\n", protocol, ret_count, *received, addr.sipc_port);
    }
    return ret_count;
}

int xenoCommunication::receiveDoubleArray(double received[], int size)
{

    //printf("inside receiving\n");
    struct sockaddr_ipc recAddr;
    int ret = 1;
    int ret_count = 0;

    //double received;
    double toReceive[size];

    while (ret > 0)
    {

        for (int i = 0; i < size; i++)
        {

            //sleep(1);
            ret = receiveSingleDouble(&toReceive[i]);
        }
        //printf("size is %d = %d\n", size, (size * sizeof(double)));
        //ret = recvfrom(s, received, sizeof(*received), MSG_DONTWAIT, (struct sockaddr *)&recAddr, &addrlen);
        //ret = recvfrom(s, &toReceive, sizeof(double), MSG_DONTWAIT, NULL, 0);
        if (ret > 0)
        {
            ret_count++;
        }
    }
    if (ret_count > 0)
    {
        for (int i = 0; i < size; i++)
        {
            received[i] = toReceive[i];
        }
    }
    if (verbose)
    {

        printf("%s - received: ", protocol);
        for (int i = 0; i < size; i++)
        {
            printf("%f", received[i]);
        }
        printf(" on port %d\n", receivePort);
    }
    return ret_count;
}

int xenoCommunication::receiveSingleDouble(double *received)
{

    //printf("inside receiving\n");
    struct sockaddr_ipc recAddr;
    int ret = 1;
    //double received;
    socklen_t addrlen;
    double toReceive;
    addrlen = sizeof(addr);
    //printf("before recvfrom\n");
    //ret = recvfrom(s, received, sizeof(*received), MSG_DONTWAIT, (struct sockaddr *)&recAddr, &addrlen);
    ret = recv(s, received, sizeof(*received), MSG_DONTWAIT);
    //printf("after...\n");

    //printf("after...\n");
    if (verbose)
    {
        printf("%s - received %d bytes, \"%f\" from port %d\n", protocol, ret, *received, addr.sipc_port);
    }
    return ret;
}

int xenoCommunication::receiveSingleDouble_WAIT(double *received)
{

    //printf("inside receiving\n");
    struct sockaddr_ipc recAddr;
    int ret = 1;
    //double received;
    socklen_t addrlen;
    double toReceive;
    addrlen = sizeof(addr);
    //printf("before recvfrom\n");
    //ret = recvfrom(s, received, sizeof(*received), MSG_DONTWAIT, (struct sockaddr *)&recAddr, &addrlen);
    ret = recv(s, received, sizeof(*received), 0);
    //printf("after...\n");

    //printf("after...\n");
    if (verbose)
    {
        printf("%s - received %d bytes, \"%f\" from port %d\n", protocol, ret, *received, addr.sipc_port);
    }
    return ret;
}
