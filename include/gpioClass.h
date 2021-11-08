#ifndef GPIOCLASS_H
#define GPIOCLASS_H

class xenoGPIO
{
public:
    xenoGPIO(int gpiopin);
    int toggle();
    int set(int value);

private:
    int GPIOWriteInit();
    int pinNumber;
    int fd;
    int initialValue;
    int pinValue;
};
#endif