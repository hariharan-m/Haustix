#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "luxcalc.h"
//these Controls are obtained from the sensor dataset
//We have to send these control ID to the sensor in order to read or write information from it
#define COMMAND 0x80
#define CONTROL 0x00
#define ON 0x03
#define OFF 0x00
#define TIMING 0x01
#define WORD 0x20
#define LOW0 0x0C
#define HIGH0 0x0D
#define LOW1 0x0E
#define HIGH1 0x0F
#define GAIN 0x00
#define ID 0x0A
#define BLOCK 0x10

int flag = 0;
/*
	This is a service application that reads data from the TSL2561 and calculates the Lux.
    Value Obtained are writte to /dev/myI2C module.
*/

int main()
{
	int file;
    int pFile2;
    int pFile1;
	volatile unsigned addr = 0x39;
	char *filename = "/dev/i2c-0";    
	unsigned buf,buf2, channel0, channel1;
	int Lux;
	int avgLux;
	char str[10];
    char level;
    char led_val[2];
	int loop;
	int avg;
	avg = 3;
	if((file = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open\n");
		exit(1);
	}
    pFile1 = open("/dev/myI2C", O_RDWR);
    pFile2 = open("/dev/mymotor", O_RDWR);
    if (pFile1 < 0) {
		fputs("myI2C not found\n",stderr);
		return 1;
	}

    if (pFile2 < 0) {
		fputs("motor not found\n",stderr);
		return 1;
	}
	if (ioctl(file,I2C_SLAVE_FORCE,addr) < 0)
	{
		printf("Failed\n");
		exit(1);
	}

	i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);

	i2c_smbus_write_byte_data(file,COMMAND|TIMING,GAIN|TIMING);

	buf = i2c_smbus_read_byte_data(file,COMMAND|ID);
	printf("ID %x\n",buf);
	while(1)
	{
		avgLux = 0;
		loop = 0;
		while(loop<avg)
		{
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);
			usleep(101000);
//read from ADC channel zero and one from I2C sensor
			channel0 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW0);
            channel1 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW1);
//calculate the average lux value
			Lux = CalculateLux(0,1,channel0,channel1,0);
			avgLux += Lux;
			loop++;
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,OFF);
		}
		avgLux = avgLux/avg;
        //check for the light brightness values
		if(avgLux >= 40000)
			avgLux = 500;
		else if (avgLux == 0)
			avgLux = 100;
        
  //check for the medium range light and write the data to mymotor for automatic mode control
        if(avgLux>100 && avgLux<=400 && flag == 0)
        {
            
		    sprintf(led_val,"%ds",1);
		    write(pFile2,led_val,sizeof(led_val));
            flag = 1;
        }
 //very low light conditions
        else if (avgLux <=100 && flag == 0){
            sprintf(led_val,"%ds",2);
		    write(pFile2,led_val,sizeof(led_val));
            flag = 1;
        }
//bright light
        else if(avgLux>700){
            flag = 0;        
        }
//writing to kernal module with Low High light values.
        if(avgLux<100){level = 'L';}
        else if(avgLux>100 && avgLux<=400 ){level = 'M';}
        else {level = 'H';}
        sprintf(str,"%c",level);
		write(pFile1,str,sizeof(str));
       // printf("value is %d \r\n",avgLux);	
        }
        close(pFile2);
        close(pFile1);
		close(file);
}
