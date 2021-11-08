#include "../Plant/Plant.h"
#include "../Controller/Controller.h"
#include "../include/communicationClass.h"
#include "../include/gpioClass.h"
#include "../include/threadClass.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <rtdm/ipc.h>
#include <stdlib.h>

#include <sys/syscall.h>
#include <sched.h>
/* the main function */
#define PERIOD_SECS 0			//10millisecs
#define PERIOD_NANOSECS 1000000 //200000000
#define XDDP_PORT 1

#define XDDP_REC 1
#define XDDP_SEND 2

bool verbose = false, gpio = false;

pthread_t p_controller,
	p_plant;
timer_t timer1;

void printtime(struct timespec *start)
{
	struct timespec tp;
	if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
		perror("clock_gettime");
	if (tp.tv_nsec - start->tv_nsec < 0)
	{
		printf(" --- %lds %ldns\n", (tp.tv_sec - start->tv_sec - 1), (1000000000 + (tp.tv_nsec - start->tv_nsec)));
	}
	else
	{
		printf(" --- %lds %ldns ---\n", (tp.tv_sec - start->tv_sec), (tp.tv_nsec - start->tv_nsec));
	}
}

void inittimer(sigset_t *set)
{
	struct itimerspec new_value, old_value;
	struct sigaction action;
	struct sigevent sevent;

	int signum = SIGALRM;

	sevent.sigev_notify = SIGEV_THREAD_ID;
	sevent.sigev_notify_thread_id = syscall(__NR_gettid);
	sevent.sigev_signo = signum;

	sigemptyset(set);
	sigaddset(set, signum);
	sigprocmask(SIG_BLOCK, set, NULL);

	if (timer_create(CLOCK_MONOTONIC, &sevent, &timer1) == -1)
		perror("timer_create");

	new_value.it_interval.tv_sec = PERIOD_SECS;
	new_value.it_interval.tv_nsec = PERIOD_NANOSECS;
	new_value.it_value.tv_sec = PERIOD_SECS;
	new_value.it_value.tv_nsec = PERIOD_NANOSECS;

	if (timer_settime(timer1, 0, &new_value, &old_value) == -1)
		perror("timer_settime");
}

static void printaff(pid_t pid)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	//pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
	sched_getaffinity(pid, sizeof(cpuset), &cpuset);
	printf("	Affinity of \"%d\" : ", pid);
	for (unsigned i = 0; i < 3; ++i)
	{
		if (CPU_ISSET(i, &cpuset))
			printf("%u", i);
	}
	printf("\n");
}

static void setaffinity(int cpu, pid_t pid)
{
	int ret;

	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);

	CPU_SET(cpu, &cpuset);

	ret = sched_setaffinity(pid, sizeof(cpuset), &cpuset);
	printf("Set affinity to %d, return code: %d\n", cpu, ret);
	printaff(pid);
}

static void *Controller_F(void *arg)
{
	int ret, sendSocket, receiveSocket;
	double receivedXDDP = 0, receivedIDDP = 0;
	IDDPconnection control_iddp(12);
	XDDPconnection control_xddp(XDDP_REC); //port 1
	XDDPconnection output_xddp(XDDP_SEND); //port 1
	XXDouble u[1 + 1];
	XXDouble y[1 + 1];
	//##############################################
	pid_t pid = getpid();
	setaffinity(0, pid);

	//##############################################
	sigset_t set;
	int signum = SIGALRM;
	xenoGPIO controllerGPIO(22);
	struct timespec start;

	inittimer(&set);

	int portLabelRec = 1;
	double counter = 0;
	//receiveSocket = initsocket(portLabelRec, XDDP);
	u[0] = 0.0; /* input */

	y[0] = 0.0; /* output */
	Controller my20simSubmodel;
	/* Initialize the submodel itself */
	my20simSubmodel.Initialize(u, y, 0.0);
	/* Simple loop, the time is incremented by the integration method */

	// while ((xx_time < xx_finish_time) && (xx_stop_simulation == XXFALSE))
	if (sigwait(&set, &signum) == -1)
		perror("sigwait");
	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
		perror("clock_gettime");
	printtime(&start);
	printf(" -- START THE CONTROLLER \n\n");
	while (1)
	{
		if (verbose)
		{
			printf("\n");
			printtime(&start);
		}
		//ret = recvfrom(receiveSocket, &receivedXDDP, sizeof(receivedXDDP), MSG_DONTWAIT, NULL, 0);
		ret = control_xddp.receiveRecentDouble(&receivedXDDP);
		ret = control_iddp.receiveRecentDouble(&receivedIDDP);
		ret = output_xddp.sendDouble(receivedIDDP);
		u[0] = receivedIDDP;
		u[1] = receivedXDDP;

		my20simSubmodel.Calculate(u, y);

		control_iddp.sendDouble(y[0], 13);
		if (gpio)
		{
			controllerGPIO.toggle();
		}
		counter++;
		if (sigwait(&set, &signum) == -1)
			perror("sigwait");
	}

	/* Perform the final calculations */
	my20simSubmodel.Terminate(u, y);
	return NULL;
	/* perform the final calculations */
}

static void *Plant_F(void *arg)
{
	struct timespec ts;
	double received;
	IDDPconnection plant_iddp(13);

	sigset_t set;
	int signum = SIGALRM;
	int sendSocket, ret;
	inittimer(&set);
	pid_t pid = getpid();
	setaffinity(1, pid);
	ts.tv_sec = 0;

	ts.tv_nsec = 500000000; /* 500 ms */
	double toSend = 0;

	XXDouble u[1 + 1];
	XXDouble y[1 + 1];

	/* initialize the inputs and outputs with correct initial values */
	u[0] = 0.0; /* u */

	y[0] = 0.0; /* y */

	Plant my20simSubmodel;

	/* initialize the submodel itself and calculate the outputs for t=0.0 */
	my20simSubmodel.Initialize(u, y, 0.0);
	printf(" -- START THE PLANT\n");
	while (1)
	{
		//clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
		//printf("PLant...\n");
		//##############################
		ret = plant_iddp.receiveRecentDouble(&received);
		u[0] = received;
		my20simSubmodel.Calculate(u, y);

		plant_iddp.sendDouble(y[0], 12);
		//##############################
		if (sigwait(&set, &signum) == -1)
			perror("sigwait");
	}

	my20simSubmodel.Terminate(u, y);
	return NULL;
}

int main(int argc, char **argv)
{
	int opt;
	while ((opt = getopt(argc, argv, "gv")) != -1)
	{
		switch (opt)
		{
		case 'v':
			verbose = true;
			break;
		case 'g':
			gpio = true;
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: sudo %s [-g] (gpio output) [-v] (verbose)\n",
					argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	xenoThread_old plantThread(80);
	xenoThread_old controllerThread(79);

	plantThread.startThread(&Plant_F, NULL, "Plant");
	controllerThread.startThread(&Controller_F, NULL, "Controller");

	plantThread.setAffinity(1);
	controllerThread.setAffinity(1);

	pause();
	return 0;
}
