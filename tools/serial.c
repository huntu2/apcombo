/* ===============================================================
	Serial Routines
=============================================================== */
//standard header
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/signal.h>

//program header
#include <main.h>
#include <serial.h>
//#include <cmdline.h>

int getbaudrate(int baudrate);
int getdatalen(int datalen);
int getstopbit(int stopbit);
int getparity(int parity);

//--------------------------------------------
void serial_port_write(char *write_buffer){
	int bytes_written;
	size_t len = 0;

	len = strlen(write_buffer);
	bytes_written = write(portstat, write_buffer, len);
	if (bytes_written < len){
		perror("Write failed \n");
	}
}

//--------------------------------------------
int pollcom(unsigned char *buff, int size){
  int n;
  n = read(portstat, &buff, size);
  return(n);
}

//------------------------------------------
int closeport(){
    if(portstat)close(portstat);
    return 0;
}

//------------------------------------------
int openport(){

    struct termios new_port_settings,old_port_settings; //,oldstdtio,newstdtio;

    portstat=open(serial.port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    //O_RWWR read+write mode:
    //O_NOCTTY flag tells UNIX this program doesn't want to be the controlling entity for that port.
    //    If you don't specify this, the device file will be owned by you, and any input
    //    (such as keyboard abort signals and so forth) will affect your process.
    //    Programs like getty(1M) use this feature when starting the login process,
    //    but normally a user program does not want this behavior.
    //O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is at -
    //    whether the other end of the port is up and running. If you do not specify this flag,
    //    your process will be put to sleep until the DCD signal line is set to the space voltage.


    if(portstat==-1){
        fprintf(stderr,"Failed to open port %s\n",serial.port);
        exit(1);
    }

    if(verbose)printf("Port open OK %s\n",serial.port);

    //The tcgetattr function fills the termios structure you provide with the current serial port configuration.
    if(tcgetattr(portstat, &old_port_settings)==-1){
        fprintf(stderr,"Unable to read portsettings %s\n",serial.port);
        closeport();
        exit(1);
    }

    if(verbose)printf("Read portsettings OK %s\n",serial.port);

    //clean our new configuration setting
    memset(&new_port_settings,0,sizeof(new_port_settings));

    //Set the baud rates,data length, parity and stop bit
    cfsetispeed(&new_port_settings, getbaudrate(serial.baudrate));
    cfsetospeed(&new_port_settings, getbaudrate(serial.baudrate));
    //new_port_settings.c_cflag |= getbaudrate(serial.baudrate);

    //new_port_settings.c_cflag |= getdatalen(serial.datalen);
    new_port_settings.c_cflag |= getdatalen(8);
    //new_port_settings.c_cflag |= getparity(serial.parity);
    new_port_settings.c_cflag |= getparity('n');
    //new_port_settings.c_cflag |= getstopbit(serial.stop);
    new_port_settings.c_cflag |= getstopbit(1);

    //Enable the receiver and set local mode...
    new_port_settings.c_cflag |= (CLOCAL | CREAD);

    //Ignore bytes with parity errors and make terminal raw and dumb.
    new_port_settings.c_iflag = IGNPAR;
    //Raw output.
    new_port_settings.c_oflag = 0;
    //Don't echo characters because if you connect to a host it or your
    //modem will echo characters for you. Don't generate signals.
    new_port_settings.c_lflag = 0;

    new_port_settings.c_cc[VMIN] = 1;      /* block untill n bytes are received */
    new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

    //now clean the modem line and activate the settings for modem */
    tcflush(portstat, TCIFLUSH);

    //Apply new configuration using tcsetattr. The TCSANOW constant specifies that all changes should occur
    //immediately without waiting for output data to finish sending or input data to finish receiving.
    //There are other constants to wait for input and output to finish or to flush the input and output buffers.
    if(tcsetattr(portstat, TCSANOW, &new_port_settings)==-1){
        perror("Failed to set new port setting, Exiting!\n");
        exit(1);
    }else if(verbose)fprintf(stdout,"New Port setting successfully applied\n");

    return 0;
}

//------------------------------------------
int getparity(int parity){
//PARENB Parity enable.
//PARODD Odd parity, else even.
    if (parity == 'n' || parity == 'N') return 0;
    else if (parity == 'e' || parity == 'E') return (PARENB);
    else if (parity == 'o' || parity == 'O') return (PARENB | PARODD);
    else {
        fprintf (stderr,"invalid parity value %c should be E/e or O/o\n",parity);
        exit(1);
    }
}

//------------------------------------------
int getstopbit(int stopbit){
    //CSTOPB Send two stop bits, else one.
    if (stopbit==2)return CSTOPB;
    else if (stopbit ==1)return 0;
    else{
        //stopbit=1;return 0;
        fprintf(stderr,"invalid stop bit size %d should be 1 or two\n",stopbit);
        exit(1);
    }
}

//------------------------------------------
int getdatalen(int datalen){
    int dlen;
    switch (datalen){
        case 5:
            dlen = CS5;
            break;
        case 6:
            dlen = CS6;
            break;
        case 7:
            dlen = CS7;
            break;
        case 8:
            dlen = CS8;
            break;
        default:
            //dlen = CS8;
            fprintf(stderr,"Invalid data length size %d should be 5-8\n",datalen);
            exit(1);
            break;
    }
    return dlen;
}

//------------------------------------------
/* Baud Rate only included the most common use*/
int getbaudrate(int baudrate){
	int baud;

	switch(baudrate){
	    case 0 :
            baud = B0; //Hang up
            break;
	    case 50 :
            baud = B50;
            break;
        case 75 :
            baud = B75;
            break;
        case 110 :
            baud = B110;
            break;
        case 134 :
            baud = B134;
            break;
        case 150 :
            baud = B150;
            break;
        case 200 :
            baud = B200;
            break;
        case 300 :
            baud = B300;
            break;
        case 600 :
            baud = B600;
            break;
        case 1200 :
            baud = B1200;
            break;
        case 1800 :
            baud = B1800;
            break;
    	case 2400 :
    		baud = B2400;
            break;
    	case 4800 :
			baud = B4800;
            break;
		case 9600 :
			baud = B9600;
            break;
		case 19200 :
			baud = B19200;
            break;
		case 38400 :
			baud = B38400;
            break;
		case 57600 :
			baud = B57600;
            break;
		case 115200 :
			baud = B115200;
            break;
        case 230400 :
            baud = B230400;
            break;
        case 460800 :
            baud = B460800;
            break;
        case 500000 :
            baud = B500000;
            break;
        case 576000 :
            baud = B576000;
            break;
        case 921600 :
            baud = B921600;
            break;
        case 1000000 :
            baud = B1000000;
            break;
		default :
            //baud = B9600;
			fprintf(stderr,"invalid baudrate %d\n",baudrate);
        	exit(1);
		break;
    }
    return baud;
}

