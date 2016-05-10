#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>


#define MAX_BUF 64

int readADC(unsigned int pin);

#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>

#define period 100000

RT_TASK adc1_task;

void adc1(void *arg)
{
	int adc0, adc1, adc2, adc3, adc4;
	RTIME now, previous;
	long MAX = 0;
        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading right Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
		adc0 = readADC(0);
		adc1 = readADC(1);
		adc2 = readADC(2);
		adc3 = readADC(3);
		adc4 = readADC(4);

//		rt_printf("ADC0, ADC1, ADC2, ADC3, ADC4: %d, %d, %d, %d, %d \n", adc0, adc1, adc2, adc3, adc4);
        	now = rt_timer_read();
		if((long)((now - previous)%1000000) > MAX){
			MAX = (long)((now - previous)%1000000) ;
		}
		rt_printf("WCET Right Motor: %ld \n", MAX);
	}
	return;
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



void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){
        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&adc1_task, "adc1 Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&adc1_task, &adc1, 0);

        //&task, name, stack size (0 - default), priority, mode 
//        rt_task_create(&lEnc_task, "lEnc Task", 0, 50, 0);
        //&task, task function, function argument
//        rt_task_start(&lEnc_task, &lEnc, 0);


}

void catch_signal(int sig){
	//empty signal handler to allow execution of cleanup code
}
void wait_for_ctrl_c(){
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	//wait for CTRL+C
	pause();
}

void cleanup(){
	rt_task_delete(&adc1_task);
//        rt_task_delete(&lEnc_task);

}


int main(int argc, char* argv[])
{
	printf("\n Press Ctrl+c to quit\n\n");
	double timeD;
	time_t begin, end;

	init_xenomai();

	startup();

	begin = time(NULL);
	while(timeD < 3){
		end = time(NULL);
		timeD = (end - begin);	
	}
	//wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
