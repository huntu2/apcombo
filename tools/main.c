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
	if(argc < 4 ){
        printf("usage %s <device> <baudrate> <ussdcommand>\n",argv[0]);
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
    if(dump)printf("AT\n");
    serial_port_write("AT\r\n");
    len=getser(data,timeout);
    if(len){
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }
    //if(len)printhex(data,len);

    if(dump)printf("ATE0\n");
    serial_port_write("ATE0\r\n");
    len=getser(data,timeout);
    if(len){
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }   
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }
    //if(len)printhex(data,len);

    if(dump)printf("AT^CURC=0\n");
    serial_port_write("AT^CURC=0\r\n");
    len=getser(data,timeout);
    if(len){
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }
    //if(len)printhex(data,len);

    if(dump)printf("AT^USSDMODE=0\n");
    serial_port_write("AT^USSDMODE=0\r\n");
    len=getser(data,timeout);
    if(len){
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }
    //if(len)printhex(data,len);

    //sending the ussd sequence start at argv[3];
    for (i=cl+3;i<argc;i++){
        memset(buffer,0,sizeof(buffer));
        strcpy(buffer,"AT+CUSD=1,\"");
        strcat(buffer,argv[i]);
        strcat(buffer,"\",15\r\n");
        //strcat(buffer,"\"\r\n");

        if(dump){printf(buffer);}
        serial_port_write(buffer);
        len=getser(data,timeoutussd);

        //see if this is not ascii
        //skip respon daOKda+CUSSD: 1,", start at 18
        //end ,68"da at len-6

        if(len>22){
            //data received nicely, dump to stdout if only last ussd sequence
            if( showlastonly || i == (argc-1)) {
                //check wheter ascii transparent or encoded 7bit ascii
                //if(dump)fwrite(&data[18],1,len-18-6,stdout);
                k=0;
                for (j=18;j<22;j++){
                    if (((data[j] >= '0') && (data[j] <='9')) ||
                        ((data[j] >= 'A') && (data[j] <='F')) ||
                        ((data[j] >= 'a') && (data[j] <='f'))
                        ) {k++;}
                    else {k=0;break;}
                }
                if(k){
                    //data not ascii encoded 7bit ascii integer (2byte char)
                    //printf("not ascii\n");
                    //printf("length  %d bagi 4 %d sisa %d\n",(len-18-6),((len-18-6)/4),(len-18-6) % 4 );
                    //printf("\n");
                    for(k=18;k<(len-6);k+=4){
                        c=0;
                        for(j=0;j<4;j++){
                            c=c<<4;
                            if      ((data[k+j] >= '0') && (data[k+j] <='9'))c+=data[k+j]-'0';
                            else if ((data[k+j] >= 'A') && (data[k+j] <='F'))c+=data[k+j]-'A'+0x0a;
                            else if ((data[k+j] >= 'a') && (data[k+j] <='f'))c+=data[k+j]-'a'+0x0a;
                            else {perror ("failed 7bit decode\n");return 1;}
                        }
                        putchar(c);//putchar(0x20);
                    }
                    printf("\n");
                }
                //data is ascii transparent, continue
                else {fwrite(&data[18],1,len-18-6,stdout);printf("\n");}
            }//if(i==argc-1)
        }//if(len>22)
        else{
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            break;
            //goto endsession;
            //return 1;
        }
    }//for (i=3;i<argc;i++)
    
    endsession:
    //end ussd session
    if(dump)printf("AT+CUSD=2\r\n");
    serial_port_write("AT+CUSD=2\r\n");
    len=getser(data,timeout);
    if(len){
        if(dump)fwrite(data,1,len,stdout);
        pb=(char *) buffer;
        pb=strstr((char *)data,"OK");
        if(memcmp("OK",pb,2)){
            perror("ERROR data!\n");
            fwrite(data,1,len,stdout);
            return 1;
        }
    }else{
        perror("ERROR timeout!\n");
        return 1;
    }

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
