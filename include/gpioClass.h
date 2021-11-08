#ifndef GPIOCLASS_H
#define GPIOCLASS_H

class xenoGPIO
{
public:
    /**
     * @brief Construct a new xeno GPIO object
     * 
     * @param gpiopin pin which will be used by this class. 
     */
    xenoGPIO(int gpiopin);
    /**
     * @brief Toggle the specified pin. 
     * 
     * @return int: value which the pin is set to. 
     */
    int toggle();
    /**
     * @brief Set the pin to the given value. 
     * 
     * @param value Value which the pin is set to. 
     * @return int: Confirmation to which value the pin is set to. 
     */
    int set(int value);

private:
    /**
     * @brief Internal function which initalizes the write-to-GPIO. 
     * 
     * @return int 
     */
    int GPIOWriteInit();
    int pinNumber;
    int fd;
    int initialValue;
    int pinValue;
};
#endif