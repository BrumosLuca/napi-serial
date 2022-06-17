#include <stdlib.h>

#include <time.h>

#include <stdio.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>   /* File Control Definitions           */

#include <poll.h>  /* test incoming events without open port */ 

#include <termios.h> /* POSIX Terminal Control Definitions */

#include <unistd.h>  /* UNIX Standard Definitions          */ 

#include <errno.h>   /* ERROR Number Definitions           */

#include "serial.h"

#define MEX_SIZE 72

#define BAUDRATE B19200

#define PARITY PARENB

#define BITS CS7

#define STOPBITS CSTOPB

#define DEVICE "/dev/ttyUSB0"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0

#define TRUE 1

unsigned char RequestMessageDet1[] = { 2, 48, 49 , 82, 78, 48, 51, 3 };// <STX> 01 R N 0 3 <ETX>
unsigned char reqSerialNumber[] = { 2, 48, 50 , 78, 82, 48 , 52, 3}; // <STX> 0 1 N R 0 3 <ETX>
unsigned char reqFirmwareVers[] = { 2, 48, 49 , 86, 82, 48, 66, 3}; // <STX> 0 1 V R 0 B <ETX>


/* USB SERIAL PORT INIT&CLOSE */

int openAndSetTermiosDevice();  //PUBLIC 
int setInterfaceAttributes(int fd);
int testDetector(int fd); 
int closeFD(int fd); //PUBLIC

/* DATA EXCHANGE */ 

int getData(int fd, int * out); //PUBLIC
void writeR(int fd);
int readR(int * out, int fd);


/* DATA PROCESSING */
int ThermoDecode(u_int8_t v1, u_int8_t v2, u_int8_t  v3);
int Manage_Data(int* out, u_int8_t * buffer);


int ThermoDecode(u_int8_t v1, u_int8_t v2, u_int8_t  v3)
{
    u_int8_t mask1 = 15, mask2 = 96;

    u_int8_t highB = ((v2 & mask1) << 4) | (v3 & mask1);

    u_int8_t lowB = (((v1 & mask1)) | ((v1 & mask2) >> 1)) | ((v2 & mask2) << 1);

    u_int16_t result = lowB + (highB << 8);

    return result;
}

int Manage_Data(int* out, u_int8_t * buffer)
{
            u_int16_t checksum = 0;

            if (buffer[0] == 2 && buffer[MEX_SIZE - 1] == 3)
            {
                for (int i = 0; i < MEX_SIZE - 3; i++)
                {
                    checksum  += buffer[i];
                }

                char hex[12];

                u_int8_t blockCheckI = buffer[MEX_SIZE - 3];
                u_int8_t blockcheckII = buffer[MEX_SIZE - 2];

                u_int8_t hexCheckI;
                u_int8_t hexCheckII;

                snprintf(hex, 12, "%X", checksum);

                if(hex[3] == 0){
                    hexCheckI = hex[1];
                    hexCheckII = hex[2];
                }else{
                    hexCheckI = hex[2];
                    hexCheckII = hex[3];
                }

                if (! (blockCheckI == hexCheckI && blockcheckII == hexCheckII) ) 
                {
                    printf("\n CHECKSUM ERROR ");
                
                    printf("\n block Check  : %d - %d ", buffer[MEX_SIZE-3], buffer[MEX_SIZE-2] );

                    printf("\n checkSum %d ", checksum);
                        
                    printf("\n");

                    for (int i = 0; i < 12 ; i++)
                    {
                            printf(" %d - ", hex[i]);
                    }

                    return FALSE;
                }

                int pool [5][5];

                int thresholdCounter = 0, poolCounter = 0;

                for (int i = 7; i <= 66; i += 3)
                {
                    int res = ThermoDecode(buffer[i], buffer[i + 1], buffer[i + 2]);

                    pool[thresholdCounter][poolCounter] = res;

                    thresholdCounter++;

                    if (thresholdCounter == 4) { thresholdCounter = 0; poolCounter++; }
                }

                out[0] = pool[0][0];
                out[1] = pool[0][1];
                out[2] = pool[0][2];
                out[3] = pool[0][3];
                out[4] = pool[0][4];

                return TRUE;
            }else
                return FALSE;
}

void writeR(int fd) {
    write(fd, RequestMessageDet1, 8);
}

int readR(int * out, int fd) {

    u_int8_t buffer [MEX_SIZE];   /* Buffer to store the data received */

    int  bytes_read = 0;    /* Number of bytes read by the read() system call */

    bytes_read = read(fd, &buffer, sizeof buffer); /* Read the data */

    if(bytes_read == 72) return Manage_Data(out,buffer);
    else return FALSE;
}

int setInterfaceAttributes(int fd)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) != 0)
    {
        return -1;
    }

    /* clear struct for new port settings */
    //bzero(&tty, sizeof(tty)); 

    tty.c_cflag = BAUDRATE | CRTSCTS | BITS | CLOCAL | CREAD | PARITY | CSTOPB;
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    tty.c_lflag = 0;

    tty.c_cc[VTIME] = 3;   /* inter-character timer unused */

    tty.c_cc[VMIN] = 72;   /* blocking read until 72 chars received */

    tcflush(fd, TCIFLUSH);

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        return -1;
    }

    return 0;
}

/*
IN : 
    file_descriptor opened
OUT : 
    1 -> detector OK
    0 -> polling timed-out / detector not answering
   -1 -> error polling
   -2 -> error writing
*/   
int testDetector(int fd){

    struct pollfd * p_fd = malloc(sizeof( struct pollfd )) ;

    p_fd->events = POLLIN;
    p_fd->fd = fd;
    
    ssize_t res = write(fd, reqFirmwareVers, 8);

    if(res <= 0 ) return -2;

    int result = poll(p_fd, 1, 5000);
      
    free(p_fd);

    return result;
}

/*
OUT : 
    file descriptor : open success, set attributes success, test detector success
    -1 : open failed
    -2 : set attribute failed
    -3 : testing detector failed
*/
int openAndSetTermiosDevice(){

    int fd;

    fd = open(DEVICE, O_RDWR | O_NOCTTY | O_SYNC );

    if (fd == -1) return -1;
    if ( setInterfaceAttributes(fd) == -1 ) return -2;
    if ( testDetector(fd) <= 0 ) return -3; 

    return fd;
}

int getData(int fd, int * out){

    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer    */
    
    writeR(fd);

    usleep((8 + 25) * 100);             // sleep enough to transmit 8 char

    if(! readR(out,fd) ) return FALSE;
    else return TRUE;
}

int closeFD(int fd){
    return close(fd); /* Close the serial port */
}

