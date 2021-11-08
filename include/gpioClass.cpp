#include <stdio.h>
#include <rtdm/gpio.h>
#include "gpioClass.h"
#define VALUE_MAX 33

int xenoGPIO::GPIOWriteInit()
{
    char path[VALUE_MAX];

    snprintf(path, VALUE_MAX, "/dev/rtdm/pinctrl-bcm2835/gpio%d", pinNumber);
    rt_printf("open device wr_only: %s   ---\n", path);
    fd = open(path, O_WRONLY);
    if (-1 == fd)
    {
        int errsv = errno;
        rt_printf("Failed to open gpio pin %d, return %d errno %d\n", pinNumber, fd, errsv);
        return -1;
    }
    rt_printf("Succes to open pin %d\n", pinNumber);

    rt_printf("set device output mode: %s   ---\n", path);
    int ret = ioctl(fd, GPIO_RTIOC_DIR_OUT, &initialValue);
    int errsv = errno;
    if (ret)
    {
        rt_printf("Failed to set output mode to pin %d  ret %d errno %d\n", pinNumber, ret, errsv);
        return ret;
    }
    rt_printf("Succes to set output mode to pin %d\n", pinNumber);

    return 1;
}

xenoGPIO::xenoGPIO(int gpiopin)
{
    pinNumber = gpiopin;
    initialValue = 0;
    pinValue = 0;
    GPIOWriteInit();
}

int xenoGPIO::set(int value)
{

    //rt_printf("write  value %d to pin %d \n", value, pin);
    int ret = write(fd, &value, sizeof(value));
    int errsv = errno;
    if (sizeof(value) != ret)
    {
        rt_printf("Failed to write  value %d to pin %d  ret %d errno %d\n", value, pinNumber, ret, errsv);
        return -1;
    }
    return 0;
}

int xenoGPIO::toggle()
{

    //rt_printf("write  value %d to pin %d \n", value, pin);
    pinValue = pinValue ^ 1;
    int ret = write(fd, &pinValue, sizeof(pinValue));
    int errsv = errno;
    if (sizeof(pinValue) != ret)
    {
        rt_printf("Failed to write  value %d to pin %d  ret %d errno %d\n", pinValue, pinNumber, ret, errsv);
        return -1;
    }
    return 0;
}