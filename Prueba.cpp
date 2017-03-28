/*
 * SIM800 <> Raspberry Pi - Serial Interface
 * To send a SMS using SIM800 module via RPi
 * Author : Ahamed EN (ahamed.en@gmail.com)
 * Date : 20 March 2016
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

   	#include <stdio.h>
    	#include <fcntl.h>   /* File Control Definitions           */
    	#include <termios.h> /* POSIX Terminal Control Definitions */
    	#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
    	#include <errno.h>   /* ERROR Number Definitions           */


/* This is R-Pi default serial port */
#define DEFAULT_PI_SERIAL_PORT "/dev/ttyTHS0"

typedef enum _mode_t{
    RW,
    WO,
    RO
}sim_mode_t;

static inline void error(char *err)
{
    printf("Error : %s\n", err);
}

//#define ERROR(err) { printf("[%s %d] ", __func__, __LINE__); error(err); }

int open_serial_dev(char *serial_dev)
{

    if(!serial_dev){
       printf("serial_dev in NULL\n");
        return -1;
    }

    int fd;

    fd = open(serial_dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0){
        printf("Could not open the serial device\n");
        return -1;
    }
    return fd;

}

/* To initialize the serial port via termios */
int serial_init(int fd)
{

    struct termios termios;
    int ret = 0;

    ret = tcgetattr (fd, &termios);
    if(ret < 0){
        printf("tcgetattr failed\n");
        return -1;
    }

    cfsetispeed(&termios, B115200);
    cfsetospeed(&termios, B115200);

    termios.c_iflag &= ~(IXANY | IXON | IXOFF);
    termios.c_lflag &= ~(ICANON | ECHO);
    termios.c_cflag |= (CLOCAL | CREAD);

    termios.c_cc[VMIN]  = 1;
    termios.c_cc[VTIME] = 10;

    ret = tcsetattr (fd, TCSANOW, &termios);
    if(ret < 0){
        printf("tcsetattr failed\n");
        return -1;
    }


    return 0;
}

void set_cmd(char *cmd, char *cmd_str)
{
    memset(cmd, 0, sizeof(cmd));
    strcpy(cmd, cmd_str);
}

int read_port(int fd, int ret)
{
    int rv = 0;
    int i = 0;
    char buf[64];
    if(ret){
        rv = read(fd, &buf, sizeof(buf));
        printf("Read from port\t: ");
        for(i=0; i<rv; i++){
            if(buf[i] == '\n' || buf[i] == '\r') continue;
            printf("%c", buf[i]);
        }
        printf("\n");
    }
    return rv;
}

int write_port(int fd, char *cmd, int len)
{
    if(fd < 0){
        printf("Invalid fd\n");
        return -1;
    }

    printf("Writing to Port\t: %s", cmd);

    int ret = 0;
    ret = write(fd, cmd, len);
    if(ret <= 0){
        printf("write failed\n");
    }
    return ret;

}

/* Function to write and read from the port */
void set_get_cmd(int fd, sim_mode_t mode, char *cmd_str)
{
    int ret;
    char cmd[64];
    set_cmd(cmd, cmd_str);
    ret = write_port(fd, cmd, strlen(cmd));
    usleep (100000);
    if(mode == WO){
        return;
    }
    read_port(fd, ret);
    return;

}

int main(int argc, char **argv)
{

    char *serial_dev = DEFAULT_PI_SERIAL_PORT;
    int ret = 0;
    int fd;


    if(argv[1]){
        serial_dev = argv[1];
    }

    fd = open_serial_dev(serial_dev);
    if(fd < 0){
        printf("Exiting\n");
        return -1;
    }

    ret = serial_init(fd);
    if(ret < 0){
        printf("Exiting\n");
        return -1;
    }

    /* Check the SIM800 Data sheet for the commands */

    set_get_cmd(fd, RW, "AT+CSQ\r\n");
  //  set_get_cmd(fd, RW, "AT+CMGF=1\r\n");
   // set_get_cmd(fd, RW, "AT+CNMI=2,1,0,0,0\r\n");

    /* Specify the 10 digit mobile # here */
  //  set_get_cmd(fd, RW, "AT+CMGS=\"9535213788\"\r\n");

    /* Following is your text message */
 //   set_get_cmd(fd, WO, "PI_SIM800_TESTING\r\n");

  //  set_get_cmd(fd, WO, "\x1A");

  //  printf("\n");

    return 0;

}
