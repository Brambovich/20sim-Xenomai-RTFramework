#include <stdio.h>
#include <unistd.h>
#include <iterator>
#include <signal.h>
#include <vector>
#include <sys/syscall.h>

#include "../../Plant/Plant.h"
#include "../../Controller/Controller.h"

#include "../../include/multiCommClass.h"
#include "../../include/runnableClass.h"
#include "../../include/superThread.h"

bool exitbool = true;

void my_handler(int s)
{
    printf("Caught signal %d\n", s);
    exitbool = false;
}

int main()
{
    //CREATE CNTRL-C HANDLER
    signal(SIGINT, my_handler);

    //CREATE PARAM AND WRAPPER FOR CONTROLLER
    int iddpcontroller_uParam[] = {0, 1};
    int xddptiltcontroller_uParam[] = {2};
    int xddppancontroller_uParam[] = {3};

    frameworkComm *controller_uPorts[] = {
        new IDDPComm(5, -1, 2, iddpcontroller_uParam),
        new XDDPComm(10, -1, 1, xddptiltcontroller_uParam)
        // , new XDDPComm(11, -1, 1, xddppancontroller_uParam)
        };
    int controller_yParam[] = {0, 1};
    int controller_yParam_logging[] = {0};
    frameworkComm *controller_yPorts[] = {
        new IDDPComm(2, 3, 2, controller_yParam)
        ,new XDDPComm(26, 26, 1, controller_yParam_logging)
    };
    Controller *controller_class = new Controller;
    runnable *controller_runnable = new wrapper<Controller>(controller_class, controller_uPorts, controller_yPorts, 2, 2);
    controller_runnable->setSize(4, 2);
    // controller_runnable->enableGPIO(22);

    //CREATE PARAM AND WRAPPER FOR PLANT
    int plant_uParam[] = {0, 1};
    frameworkComm *plant_uPorts[] = {
        new IDDPComm(3, -1, 2, plant_uParam)};
    int plant_yParam[] = {0, 1};
    int plant_yParam_logging[] = {0};
    frameworkComm *plant_yPorts[] = {
        new IDDPComm(4, 5, 2, plant_yParam)
        , new XDDPComm(28, 28, 1, plant_yParam_logging)
    };
    Plant *plant_class = new Plant;
    runnable *plant_runnable = new wrapper<Plant>(plant_class, plant_uPorts, plant_yPorts, 1, 2);
    plant_runnable->setSize(2, 2);
    // plant_runnable->enableGPIO(23);

    //INIT XENOTRHEAD FOR CONTROLLER + PLANT
    xenoThread plantClass(plant_runnable);
    xenoThread controllerClass(controller_runnable);
    plantClass.init(1000000, 98, 1);
    controllerClass.init(1000000, 99, 0);

    plantClass.enableLogging(true, 25);
    controllerClass.enableLogging(true, 27);

    //START THREADS
    controllerClass.start("controller");
    plantClass.start("plant");

    // WAIT FOR CNTRL-C
    while (exitbool)
        ;
    printf("STOP RUNNNING...\n");

    //CLEANUP
    controllerClass.stopThread();
    plantClass.stopThread();
    controller_runnable->~runnable();
    plant_runnable->~runnable();
    return 1;
}