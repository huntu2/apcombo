#ifndef _timer_h_
#define _timer_h_

//void * timer_signal_handler(void);

int start_timer(int duration, void * timer_sig_handler);
//create void * timer_handler(void) function to handle the alarm timeout signal
int restart_timer(int duration);
//used to reset the timer upon new reception

void stop_timer(void);

#endif //_timer_h_
