#ifndef _getser_h_
#define _getser_h_
//-----------------------------------

struct _poolserial{
    char run;
    char state;
    int timeout;
} poolserial;

int getser(unsigned char * pdata, unsigned int timeout);
void * timer_signal_handler(void);

#endif //_capser_h_

