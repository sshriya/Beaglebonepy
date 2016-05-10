/*
 Read an input pin using memory map
Encoder r - A: P8_11
	    B: P8_12
Encoder l - A: P8_15
	    B: P8_16
*/
#include <fcntl.h>
#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

#define HIGH 1
#define LOW 0

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>

#define period 100000

RT_TASK rEnc_task;
RT_TASK lEnc_task;
void rEnc(void *arg)
{
 	int i = 0;
    	int rPose = 0;
    	int r_last = LOW;
    	int inr = 0;
    	int inrB = 0;
    	char logicA = '1';
    	char logicB = '2';
    	int fd = open("/dev/mem",O_RDWR | O_SYNC);
    	ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
    	//configure encoder pins as input
    	pinconf1[OE_ADDR/4]  |= ((1<<13)|(1 << 12)); //P8_11, P8_12, P8_15, P8_16   

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading right Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);

                if(pinconf1[GPIO_DATAIN/4] & (1 << 13)){
                        inr = HIGH;
                }else{
                        inr = LOW;
                }


		if((r_last == LOW)&&(inr == HIGH)){
			if(pinconf1[GPIO_DATAIN/4] & (1 << 12)) {
        	                inrB = HIGH;
                	}else{
                        	inrB = LOW;
                	}

			if(inrB == LOW){
				rPose--;
			}else{
				rPose++;
			}
		}
		r_last = inr;
		rt_printf("Right ticks: %d \n", rPose);
        }
	return;
}

void lEnc(void *arg)
{
        int i = 0;
        int lPose = 0;
        int r_last = LOW;
        int inr = 0;
        int inrB = 0;
        char logicA = '1';
        char logicB = '2';
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
        //configure encoder pins as input
        pinconf1[OE_ADDR/4]  |= ((1<<15)|(1<<14)); //P8_11, P8_12, P8_15, P8_16   

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading Left Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);

                if(pinconf1[GPIO_DATAIN/4] & (1 << 15)){
                        inr = HIGH;
                }else{
                        inr = LOW;
                }

		 if((r_last == LOW)&&(inr == HIGH)){
                        if(pinconf1[GPIO_DATAIN/4] & (1 << 14)) {
                                inrB = HIGH;
                        }else{
                                inrB = LOW;
                        }

                        if(inrB == LOW){
                                lPose--;
                        }else{
                                lPose++;
                        }
                }
                r_last = inr;
                rt_printf("Left ticks: %d \n", lPose);
        }
        return;
}


void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){
        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&rEnc_task, "rEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&rEnc_task, &rEnc, 0);

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
	rt_task_delete(&rEnc_task);
//        rt_task_delete(&lEnc_task);

}


int main(int argc, char* argv[])
{
	printf("\n Press Ctrl+c to quit\n\n");

	init_xenomai();

	startup();

	wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
