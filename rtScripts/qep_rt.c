#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>


#define MAX_BUF 64

#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>

#define period 100000

RT_TASK rEncoder_task;
RT_TASK lEncoder_task;

void rEncoder(void *arg)
{
	long rEnc, lEnc, fd, rBias;
        char buf[MAX_BUF];
        char val[14]; //stores 4 digits ADC value
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/48302000.epwmss/48302180.eqep/position");
	RTIME now, previous;
	long MAX = 0;
        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading right Encoder!\n");
	fd = open(buf, O_RDONLY); //Open ADC as read only

                if(fd < 0){
                        perror("Problem opening right Encoder");
                }

         read(fd, &val, 15); //read upto 4 digits 0-1799
         close(fd);

         rBias = atoi(val); //return integer value

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
		fd = open(buf, O_RDONLY); //Open ADC as read only

        	if(fd < 0){
                	perror("Problem opening ADC");
        	}

        	read(fd, &val, 15); //read upto 4 digits 0-1799
        	close(fd);

        	rEnc = rBias - atoi(val); //return integer value

        	rt_printf("right Encoder ticks: %d\n", rEnc);
		now = rt_timer_read();
		if((long)((now - previous)%1000000) > MAX){
			MAX = (long)((now - previous)%1000000) ;
		}
//		rt_printf("WCET Right Motor: %ld \n", MAX);
	}
	return;
}

void lEncoder(void *arg)
{
        int rEnc, lEnc, fd, lBias;
        char buf[MAX_BUF];
        char val[15]; //stores 4 digits ADC value
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/48304000.epwmss/48304180.eqep/position");
        RTIME now, previous;
        long MAX = 0;
        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading Left Encoder!\n");
        fd = open(buf, O_RDONLY); //Open ADC as read only

                if(fd < 0){
                        perror("Problem opening left Encoder");
                }

         read(fd, &val, 15); //read upto 4 digits 0-1799
         close(fd);

         lBias = atoi(val); //return integer value

        while (1){
                rt_task_wait_period(NULL);
                previous = rt_timer_read();
                fd = open(buf, O_RDONLY); //Open ADC as read only

                if(fd < 0){
                        perror("Problem opening ADC");
                }

                read(fd, &val, 15); //read upto 4 digits 0-1799
                close(fd);

                lEnc = lBias - atoi(val); //return integer value

                rt_printf("Left Encoder ticks: %d\n", lEnc);
                now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
  //              rt_printf("WCET left Motor: %ld \n", MAX);
        }
        return;
} 
 
void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){
        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&rEncoder_task, "rEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&rEncoder_task, &rEncoder, 0);

        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&lEncoder_task, "lEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&lEncoder_task, &lEncoder, 0);


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
	rt_task_delete(&rEncoder_task);
        rt_task_delete(&lEncoder_task);

}


int main(int argc, char* argv[])
{
	printf("\n Press Ctrl+c to quit\n\n");
	double timeD;
	time_t begin, end;

	init_xenomai();

	startup();

	begin = time(NULL);
	while(timeD < 10){
		end = time(NULL);
		timeD = (end - begin);	
	}
	//wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
