#ifndef COMCLASS_H
#define COMCLASS_H

#include <rtdm/ipc.h>

/**
* Upper communication class describing the receiving and sending functions applicable to both IDDP and XDDP.
* @brief Xenomai ased communication class.
*/
class xenoCommunication
{
public:
    /**
    * @brief Xenomai IPC based send function.
    * Double will be send to the supplied port. Function is set to MSG_DONTWAIT, therefore if no room in XDDP/IDDP buffer, send will be skipped.
    * @param toSend double value which needs to be sent.
    * @param port XDDP/IDDP destination port
    * @return return value of the send function, -1 if failed.
    * @see sendDouble(double toSend)
    * @see sendDouble(double toSend, int port, bool verbose)
    * @see sendDouble(double toSend, bool verbose)
    */
    int sendDouble(double toSend, int port);
    /**
    * @brief Xenomai IPC based send function.
    * @param toSend double value which needs to be sent.
    * Function will send the supplied double to the port set by the initializer of the class, best used within XDDP.
    * @return return value of the send function, -1 if failed.
    * @see sendDouble(double toSend, int port)
    * @see sendDouble(double toSend, int port, bool verbose)
    * @see sendDouble(double toSend, bool verbose)
    */
    int sendDoubleArray(double toSend[], int port, int size);
    int sendDouble(double toSend);
    /**
    * @brief Xenomai IPC based send function.
    * @param toSend double value which needs to be sent.
    * @param port XDDP/IDDP destination port
    * @param verbose Depending on true or false, the function will print the send value with the port.
    * supplied toSend value will be send to the port supplied. The function will print the send value if verbose is set to true. 
    * @return return value of the send function, -1 if failed.
    * @see sendDouble(double toSend, int port)
    * @see sendDouble(double toSend, int port, bool verbose)
    * @see sendDouble(double toSend, bool verbose)
    */
    int sendDouble(double toSend, int port, bool verbose);
    /**
    * @brief Xenomai IPC based send function.
    * @param toSend double value which needs to be sent.
    * @param verbose Depending on true or false, the function will print the send value with the port.
    * supplied toSend value will be send to the port set by the class initializer. The function will print the send value if verbose is set to true. 
    * @return return value of the send function, -1 if failed.
    * @see sendDouble(double toSend, int port)
    * @see sendDouble(double toSend, int port, bool verbose)
    * @see sendDouble(double toSend, bool verbose)
    */
    int sendDouble(double toSend, bool verbose);
    int receiveDoubleArray(double *received, int size);
    int receiveRecentDouble(double *received, bool verbose);
    int receiveSingleDouble(double *received, bool verbose);
    int receiveRecentDouble(double *received);
    int receiveSingleDouble(double *received);

protected:
    int receivePort;
    const char *protocol;
    int s;
    struct sockaddr_ipc addr;
};

class IDDPconnection : public xenoCommunication
{
public:
    IDDPconnection(int r_recP);
    ~IDDPconnection();

private:
    void initSocket();
};

class XDDPconnection : public xenoCommunication
{
public:
    XDDPconnection(int r_recP);
    ~XDDPconnection();

private:
    void initSocket();
};

#endif
