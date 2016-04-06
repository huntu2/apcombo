#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#include <timer.h>

//----------------------------------------------------
struct sigaction newtimer_handler, oldtimer_handler;
//void * timer_signal_handler(void){};

//----------------------------------------------------
int restart_timer(int duration){
    struct itimerval timerval;

    timerval.it_interval.tv_sec = duration / 1000;
    timerval.it_interval.tv_usec = (duration % 1000) * 1000;
    timerval.it_value.tv_sec = duration / 1000;
    timerval.it_value.tv_usec = (duration % 1000) * 1000;
    if(setitimer(ITIMER_REAL, &timerval, NULL))
    {
        perror("setitimer() error\n");
        exit(1);
    }
    return(0);
}

//----------------------------------------------------
int start_timer(int duration, void * timer_sig_handler){
    struct itimerval timerval;

    timerval.it_interval.tv_sec = duration / 1000;
    timerval.it_interval.tv_usec = (duration % 1000) * 1000;
    timerval.it_value.tv_sec = duration / 1000;
    timerval.it_value.tv_usec = (duration % 1000) * 1000;
    if(setitimer(ITIMER_REAL, &timerval, NULL))
    {
        perror("setitimer() error\n");
        exit(1);
    }

    newtimer_handler.sa_handler = timer_sig_handler;
    newtimer_handler.sa_flags = SA_NOMASK;
    if(sigaction(SIGALRM, &newtimer_handler, &oldtimer_handler))
    {
        printf("\nsigaction() error\n");
        exit(1);
    }
    return(0);
}

//----------------------------------------------------
void stop_timer(void)
{
    struct itimerval timerval;

    timerval.it_interval.tv_sec = 0;
    timerval.it_interval.tv_usec = 0;
    timerval.it_value.tv_sec = 0;
    timerval.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timerval, NULL);

    //restore old signal handler
    sigaction(SIGALRM, &oldtimer_handler, NULL);
}

