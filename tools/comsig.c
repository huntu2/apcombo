/*
*   USSD command special for huawei modem
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <main.h>
#include <serial.h>
#include <getser.h>
#include <timer.h>

#define timeout 1
#define timeoutussd 3

int printhex(unsigned char * pstr, unsigned int len);

//===========================================
int main (int argc, char * argv[])
{
    //------------------
    //debugging option
    char dump,showlastonly;

    //-------------------
    int i,j,k,len,cl;
    unsigned int c;
    unsigned char data[1024];
    char buffer[1024];
    char * pb;

	//evaluate cmdline line argument
	if(argc < 3 ){
        printf("usage %s <device> <baudrate>\n",argv[0]);
	    return 1;
	}

	//dump verbose option
	if     (!memcmp("-vvv",argv[1],4)){cl=1;showlastonly=1;dump=1;verbose=1;}
	else if(!memcmp("-vv" ,argv[1],3)){cl=1;showlastonly=1;dump=1;verbose=0;}
	else if(!memcmp("-v"  ,argv[1],2)){cl=1;showlastonly=1;dump=0;verbose=0;}
    else {cl=0;showlastonly=0,dump=0,verbose=0;}

    //setting up baudrate
    serial.baudrate=atoi(argv[cl+2]);
    if(getbaudrate(serial.baudrate)==1){
        fprintf(stderr,"Fatal Error, cannot set baudrate: %d\n",serial.baudrate);
        return 1;
    }

	serial.port=strdup(argv[cl+1]);
    //opening the port
    if (openport()){
        fprintf(stderr,"Fatal Error, cannot open device: %s\n",serial.port);
        return 1;
    }

    ///*
    if(dump)printf("AT+CSQ\n");
    serial_port_write("AT+CSQ\r\n");
    len=getser(data,timeout);
    if(len){
        system("clear");
        fwrite(data,1,len,stdout);
        /*
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }
        */
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }
    //if(len)printhex(data,len);

    closeport();
    return 0;
}
//===========================================
 int printhex(unsigned char * pstr, unsigned int len){
    int i;

    for (i=0;i<len;i++){
        if(*pstr > 0x0f)fprintf(stdout,"%x ",*pstr++);
        else if(*pstr > 0)fprintf(stdout,"0%x ",*pstr++);
        else {printf("00 ");pstr++;}
    }
    printf("\n");

    return 0;
 }
