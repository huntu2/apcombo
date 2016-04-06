#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <main.h>
#include <serial.h>
#include <timer.h>
#include <getser.h>

//#define readone
#undef readone

//------------------------------------------------------------------------
int getser(unsigned char * pdata, unsigned int timeout){
    int i,j,len=0;
    //unsigned char * pbak=pdata;

    #ifdef readone
        unsigned char c;
    #else
        unsigned char c[128];
    #endif

    //resetting up time out
    poolserial.timeout=timeout*10;
    //start timer with 100mS time tick
    start_timer(100, timer_signal_handler);
    while(poolserial.timeout){
        //start receiving new data stream
        #ifdef readone
            i=read(portstat,&c,1);
        #else
            i=read(portstat,&c,sizeof(c));
        #endif

        if(i >0 && i !=-1){

            //write stream data
            #ifdef readone
                *pdata++ = c;
            #else
            for (j=0;j<i;j++){
                *pdata++=c[j];
            }
            #endif
            len+=i;

            //dump
            //write(1,&c,i);

            //searching for 0d 0a 4f 4d 0d 0a

            //resetting up time out
            poolserial.timeout=timeout*10;
            //start timer with 100mS time tick
            start_timer(100, timer_signal_handler);
        }

    }//while(poolserial.timeout)

    return len;
}

//------------------------------------------------------------------------
void * timer_signal_handler(void){
    if(poolserial.timeout){
        poolserial.timeout--;
    }
    return(0);
}
