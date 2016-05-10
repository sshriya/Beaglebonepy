#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
 
int main(){
	const char *fname = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
        int count = 0, fd, len;
        char adc[5] = {0};
 
         while(count < 3000){
                fd = open(fname, O_RDONLY);
                if(fd == -1){
                        printf("error: %s %d\n", strerror(errno), errno);
                        exit(1);
	                }
 
                if(count % 10 == 0 && count != 0)
                        printf("\n");
 
	                len = read(fd, adc, sizeof(adc - 1));
 
	                if(len == -1){
	                        close(fd);
	                        continue;               
	                }
	                else if(len == 0){
	                        printf("%s\n", "buffer is empty");
	                }
	                else{
	                        adc[len] ='\0';
	                        printf("%s ", adc);
	                }
 
	                close(fd);
 
	                count++;
	        }
 
	        return  0;
	}
