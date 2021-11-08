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
    int sendDouble(double toSend);
    /**
    * @brief Xenomai IPC based send function for arrays.
    * array will be send to the supplied port. Function is set to MSG_DONTWAIT, therefore if no room in XDDP/IDDP buffer, send will be skipped.
    * @param toSend double array value which needs to be sent.
    * @param size size of the double array. 
    * @param port XDDP/IDDP destination port
    * @return return value of the send function, -1 if failed.
    * @see sendDouble(double toSend)
    * @see sendDouble(double toSend, int port, bool verbose)
    * @see sendDouble(double toSend, bool verbose)
    */
    int sendDoubleArray(double toSend[], int port, int size);
    /**
     * @brief Set the class to be silent or explicit. 
     * 
     * @param _verbose boolean which indicates verbosity. 
     */
    void setVerbose(bool _verbose);
    /**
     * @brief receive an array from the pre-specified port. 
     * 
     * @param received pointer on which the received value will be written.
     * @param size size of the to-be-received value. 
     * @return int: size of the received value. 
     */
    int receiveDoubleArray(double *received, int size);
    /**
     * @brief receive the most recently sent double in the XDDP/IDDP buffer. (entire buffer is emptied)
     * 
     * @param received pointer on which the received value will be written.
     * @return int: size of the received value. 
     */
    int receiveRecentDouble(double *received);
    /**
     * @brief receive the oldest sent double in the XDDP/IDDP buffer. (only last value is removed)
     * 
     * @param received pointer on which the received value will be written.
     * @return int: size of the received value. 
     */
    int receiveSingleDouble(double *received);
    /**
     * @brief receive the oldest sent double in the XDDP/IDDP buffer. (only last value is removed)
     * 
     * @param received pointer on which the received value will be written.
     * @return int: size of the received value. 
     */
    int receiveSingleDouble_WAIT(double *received);
    /**
     * @brief Virtual function that is overwritten with individual XDDP/IDDP initial socket functionality. 
     * 
     */
    virtual void initSocket() = 0;
    virtual ~xenoCommunication() {}

protected:
    int receivePort;
    const char *protocol;
    bool verbose = false;
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
