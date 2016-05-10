#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_BUF 64

int readADC(unsigned int pin);
 
int main(){
	int adc0 = readADC(0);
	int adc1 = readADC(1);
	printf("ADC0 : %d\n", adc0);
        printf("ADC1 : %d\n", adc1);

	return 0;
}

int readADC(unsigned int pin){
	int fd; //file pointer
	char buf[MAX_BUF];
	char val[4]; //stores 4 digits ADC value

	snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", pin);
	fd = open(buf, O_RDONLY); //Open ADC as read only

	if(fd < 0){
		perror("Problem opening ADC");
	}

	read(fd, &val, 4); //read upto 4 digits 0-1799
	close(fd);

	return atoi(val); //return integer value

}
