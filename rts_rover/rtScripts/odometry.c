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
#include <native/queue.h>
#include <native/sem.h>
#include <math.h>

#define period 100000

RT_TASK rEnc_task;
RT_TASK lEnc_task;
RT_TASK Odo_task;

RT_SEM rsem;
RT_SEM lsem;

//robot dimension
#define R 30
#define L 185
#define N 250/3 //Ticks per revolution
#define pi 3.14159
#define m_per_tick 2*pi*R/N

#define QUEUE_SIZE 255
RT_QUEUE rqueue;
RT_QUEUE lqueue;

void rEnc(void *arg)
{
    	int r_last = LOW;
    	int inr = 0;
    	int inrB = 0;
	int rtick = 0;
    	char rticks[40];
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
				rtick--;
			}else{
				rtick++;
			}
		}
		r_last = inr;
		//rt_printf("Right ticks: %d \n", rtick);
		
		//send to odometry task
		sprintf(rticks, "%d", rtick);
		rt_queue_write(&rqueue, rticks, sizeof(rticks), Q_NORMAL);
        }
	return;
}

void lEnc(void *arg)
{
        int l_last = LOW;
        int inrl = 0;
        int inrBl = 0;
        int ltick = 0;
	char lticks[40];

        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
        //configure encoder pins as input
        pinconf1[OE_ADDR/4]  |= ((1<<15)|(1<<14)); //P8_11, P8_12, P8_15, P8_16   

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading Left Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);

                if(pinconf1[GPIO_DATAIN/4] & (1 << 15)){
                        inrl = HIGH;
                }else{
                        inrl = LOW;
                }

		 if((l_last == LOW)&&(inrl == HIGH)){
                        if(pinconf1[GPIO_DATAIN/4] & (1 << 14)) {
                                inrBl = HIGH;
                        }else{
                                inrBl = LOW;
                        }

                        if(inrBl == LOW){
                                ltick--;
                        }else{
                                ltick++;
                        }
                }
                l_last = inrl;
                //rt_printf("Left ticks: %d \n", ltick);
		
                //send to odometry task
                sprintf(lticks, "%d", ltick);
                rt_queue_write(&lqueue, lticks, sizeof(lticks), Q_NORMAL);


        }
        return;
}

void Odo(void *arg){

	double rtick_prev = 0;
	double ltick_prev = 0;
	double dtick_r = 0;
	double dtick_l = 0;
	char r_mesg[40], l_mesg[40];
	int r_tick, l_tick;

        double Dr, Dc, Dl, x, y, theta, x_dt, y_dt, theta_dt, x_new, y_new, theta_new;

	Dr = Dc = Dl = 0;
	x = y = theta = 0;
	x_new=y_new=theta_new = 0;
	x_dt = y_dt = theta_dt = 0;

	rt_task_set_periodic(NULL, TM_NOW, period);	

	while(1){
		rt_task_wait_period(NULL);
	
		rt_queue_read(&rqueue, r_mesg, sizeof(r_mesg), TM_INFINITE);
		rt_queue_read(&lqueue, l_mesg, sizeof(l_mesg), TM_INFINITE);

		r_tick = atoi(r_mesg);
		l_tick = atoi(l_mesg);

		//rt_printf("int data: %d \n", l_tick);
		//rt_printf("data r : %d \n", r_tick);

		dtick_r = r_tick - rtick_prev;
                dtick_l = l_tick - ltick_prev;

		Dr = m_per_tick*dtick_r;
                Dl = m_per_tick*dtick_l;
		Dc = (Dr+Dl)/2;

		x_dt = Dc*cos(theta);
		y_dt = Dc*sin(theta);
		theta_dt = (Dr-Dl)/L;

		theta_new = theta + theta_dt;
		x_new = x + x_dt;
		y_new = y + y_dt;

		rtick_prev = r_tick;
		ltick_prev = l_tick;

		x = x_new;
		y = y_new;
		theta = theta_new;

		rt_printf("Robot pose (x, y, theta) is: %lf, %lf, %lf\n", x, y, theta);
		

	}

}

void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){

	rt_queue_create(&rqueue, "rQueue", QUEUE_SIZE, 40, Q_FIFO);
        rt_queue_create(&lqueue, "lQueue", QUEUE_SIZE, 40, Q_FIFO);

	//rt_sem_create(&rsem, "rsem", 0, S_FIFO);
        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&rEnc_task, "rEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&rEnc_task, &rEnc, 0);

        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&lEnc_task, "lEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&lEnc_task, &lEnc, 0);


        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&Odo_task, "Odo Task", 0, 60, 0);
        //&task, task function, function argument
        rt_task_start(&Odo_task, &Odo, 0);

	//rt_sem_broadcast(&rsem);

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
        rt_task_delete(&lEnc_task);
	rt_task_delete(&Odo_task);

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
