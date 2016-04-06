#ifndef _serial_h_
#define _serial_h_

int openport();
int closeport();
int pollcom(unsigned char *buf, int size);
void serial_port_write(char *write_buffer);
int getbaudrate(int baudrate);

int portstat;

struct _serial{
	char * port;
	int baudrate;
} serial;


#endif //_serial_h_
