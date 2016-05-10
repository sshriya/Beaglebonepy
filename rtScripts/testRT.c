/* 
Application to test a Real time task with a non-RT task execution. This program makes the robot move in a circular trajectory. 
For a non-RT task, due to dummy tasks, the CPU is overloaded and the execution of circular trjectory fails.

Date: April, 26th , 2016
Shriya Shah & Arpit Goyal
Control a DC motor:
Motor 1 & 3: Right 
Logic 1 - P9_12 - GPIO1[28]
Logic 2 - P9_15 - GPIO1[16]
PWM 1 - P9_23 - GPIO1[17]

Motor 2 & 4: Left
Logic 1 - P8_7 - GPIO2[2]
Logic 2 - P8_8 - GPIO2[3]
PWM 1 - P8_9 - GPIO2[5]

*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>

#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

#define period1 800000
#define period2 800000

#define duty1 period1*0.25
#define duty2 period2*0.75

#define p1 10000
#define p2 10000

RT_TASK rMotor_task;
RT_TASK lMotor_task;
RT_TASK stop_rMotor;
RT_TASK stop_lMotor;
RT_TASK dummy_task1;
RT_TASK dummy_task2;
RT_TASK dummy_task3;
RT_TASK dummy_task4;

void stoprMotor(void *arg){
	int fd = open("/dev/mem",O_RDWR | O_SYNC);
    	ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);	
	pinconf1[OE_ADDR/4] &= pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1 << 28)|(1<<16)|(1<<17))); 
	//configure logic pins
	rt_printf("stop r motor\n");
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 28); //clear pin  P9_12
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 16); // clear pin P9_15
        pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 17); // clear pin P9_15
}

void stoplMotor(void *arg)
{
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf2 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_ADDR);     
        pinconf2[OE_ADDR/4] &= pinconf2[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1<<2)|(1 << 3)|(1<<5))); 
        //configure logic pins
	rt_printf("stop l motor\n");
        pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 2); //set pin  P8_7
     	pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 3); // clear pin P8_8
        pinconf2[GPIO_DATAOUT/4] &= ~(1<<5);
}
void rMotor(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	RTIME now, previous;
	long MAX = 0;

	int fd = open("/dev/mem",O_RDWR | O_SYNC);
    	ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);	
	pinconf1[OE_ADDR/4] &= pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1 << 28)|(1<<16)|(1<<17))); 
	//configure logic pins
     	pinconf1[GPIO_DATAOUT/4]  |= (1 << 28); //set pin  P9_12
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 17); // clear pin P9_15
	rt_task_set_periodic(NULL, TM_NOW, period1);
        rt_printf("Controling Right Motors!\n");
//	previous = rt_timer_read();
	while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
		pinconf1[GPIO_DATAOUT/4] |= (1 << 17); //PWM on pin P9_23
		rt_task_sleep(duty1);
		pinconf1[GPIO_DATAOUT/4] &= ~(1 << 17); //toggle pin
//		rt_printf("right Motor PWM, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
		now = rt_timer_read();
		if((long)((now - previous)%1000000) > MAX){
			MAX = (long)((now - previous)%1000000) ;
		}
		rt_printf("WCET Right Motor: %ld \n", MAX);
		//previous = now;
	}
}

void lMotor(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        RTIME now, previous;
	long MAX = 0;
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf2 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_ADDR);     
        pinconf2[OE_ADDR/4] &= pinconf2[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1<<2)|(1 << 3)|(1<<5))); 
        //configure logic pins
        pinconf2[GPIO_DATAOUT/4]  |= (1 << 2); //set pin  P8_7
     	pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 3); // clear pin P8_8
        rt_task_set_periodic(NULL, TM_NOW, period2);
        rt_printf("Controling Left Motors!\n");
        //previous = rt_timer_read();
        while (1){
                rt_task_wait_period(NULL);
                previous = rt_timer_read();
                pinconf2[GPIO_DATAOUT/4] |= (1 << 5); //PWM on pin P8_9
                rt_task_sleep(duty2);
                pinconf2[GPIO_DATAOUT/4] &= ~(1 << 5); //toggle pin
  //              rt_printf("Left Motor PWM, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000); 
                now = rt_timer_read();
		if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET Left Motor: %ld \n", MAX);

		//previous = now;
        }
}

//Periodic task
void dummy1(void *arg)
{	
	int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p1);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 100000000; i++);
//		rt_printf("Executing dummy task 1\n");
        }
}

void dummy2(void *arg)
{	
	int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p2);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 100000000; i++);
  //              rt_printf("Executing dummy task 2\n");
        }
}

void dummy3(void *arg)
{       
        int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p2);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 100000000; i++);
  //              rt_printf("Executing dummy task 2\n");
        }
}

void dummy4(void *arg)
{       
        int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p2);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 100000000; i++);
  //              rt_printf("Executing dummy task 2\n");
        }
}



void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

int startup(){
    	char str[10];
		
	sprintf(str, "rMotor");
	//&task, name, stack size (0 - default), priority, mode 
	rt_task_create(&rMotor_task, str, 0, 50, 0);
	//&task, task function, function argument
	rt_task_start(&rMotor_task, &rMotor, 0);

        sprintf(str, "lMotor"); 
        rt_task_create(&lMotor_task, str, 0, 50, 0);
        rt_task_start(&lMotor_task, &lMotor, 0);

        sprintf(str, "StoplMotor"); 
        rt_task_create(&stop_lMotor, str, 0, 90, 0);

        sprintf(str, "StoprMotor"); 
        rt_task_create(&stop_rMotor, str, 0, 90, 0);

        rt_task_create(&dummy_task1, "dummy 1", 0, 0, 0);
        rt_task_start(&dummy_task1, &dummy1, 0);
        rt_task_create(&dummy_task2, "dummy 2", 0, 0, 0);
        rt_task_start(&dummy_task2, &dummy2, 0);


        rt_task_create(&dummy_task3, "dummy 3", 0, 0, 0);
        rt_task_start(&dummy_task3, &dummy3, 0);
        rt_task_create(&dummy_task4, "dummy 4", 0, 0, 0);
        rt_task_start(&dummy_task4, &dummy4, 0);

}

void catch_signal(int sig){
}

void wait_for_ctrl_c(){
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	//wait for CTRL+C
	pause();
}

void cleanup(){

        rt_task_start(&stop_lMotor, &stoplMotor, 0);
        rt_task_start(&stop_rMotor, &stoprMotor, 0);

        rt_task_delete(&lMotor_task);
        rt_task_delete(&rMotor_task);
	rt_task_delete(&stop_lMotor);
	rt_task_delete(&stop_rMotor);
	rt_task_delete(&dummy_task1);
	rt_task_delete(&dummy_task2);
}

int main(int argc, char* argv[])
{

	double timeD;
	time_t begin, end;
	printf("\n Press Ctrl+c to quit\n\n");

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

