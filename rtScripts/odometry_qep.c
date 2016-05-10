/*
 Read an input pin using memory map
Encoder r - A: P8_11
	    B: P8_12
Encoder l - A: P8_15
	    B: P8_16
*/
#include <fcntl.h>

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

#define MAX_BUF 64

RT_TASK rEncoder_task;
RT_TASK lEncoder_task;
RT_TASK Odo_task;

RT_SEM rSem;
RT_SEM lSem;

int rtick = 0;
int ltick = 0;

//robot dimension
#define R 30
#define L 185
#define N 100/3 //Ticks per revolution
#define pi 3.14159
#define m_per_tick 2*pi*R/N

#define QUEUE_SIZE 255
RT_QUEUE rqueue;
RT_QUEUE lqueue;

void rEncoder(void *arg)
{
	int rEnc, lEnc, fd;
        char buf[MAX_BUF];
        char val[4]; //stores 4 digits ADC value
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/48302000.epwmss/48302180.eqep/position");
	RTIME now, previous;
	long MAX = 0;
        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading right Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
		fd = open(buf, O_RDONLY); //Open ADC as read only

        	if(fd < 0){
                	perror("Problem opening right Encoder");
        	}

        	read(fd, &val, 4); //read upto 4 digits 0-1799
        	close(fd);

        	rEnc = atoi(val); //return integer value
		
		rt_sem_p(&rSem, 0);
		rtick = rEnc;
		rt_sem_v(&rSem);
        	rt_printf("right Encoder ticks: %d\n", rtick);
		now = rt_timer_read();
		if((long)((now - previous)%1000000) > MAX){
			MAX = (long)((now - previous)%1000000) ;
		}
		//rt_printf("WCET Right Motor: %ld \n", MAX);
//		rt_queue_write(&rqueue, rEnc, sizeof(rEnc), Q_NORMAL);
	}
	return;
}


void lEncoder(void *arg)
{
        int rEnc, lEnc, fd;
        char buf[MAX_BUF];
        char val[4]; //stores 4 digits ADC value
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/48304000.epwmss/48304180.eqep/position");
        RTIME now, previous;
        long MAX = 0;
        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading Left Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);
                previous = rt_timer_read();
                fd = open(buf, O_RDONLY); //Open ADC as read only

                if(fd < 0){
                        perror("Problem opening ADC");
                }

                read(fd, &val, 4); //read upto 4 digits 0-1799
                close(fd);

                lEnc = atoi(val); //return integer value
                rt_sem_p(&lSem, 0);
                ltick = lEnc;
                rt_sem_v(&lSem);

                rt_printf("Left Encoder ticks: %d\n", ltick);
                now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET left Motor: %ld \n", MAX);
        	//rt_queue_write(&lqueue, lEnc, sizeof(lEnc), Q_NORMAL);

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
	
//		rt_queue_read(&rqueue, r_mesg, sizeof(r_mesg), TM_INFINITE);
//		rt_queue_read(&lqueue, l_mesg, sizeof(l_mesg), TM_INFINITE);

//		r_tick = atoi(r_mesg);
//		l_tick = atoi(l_mesg);

		r_tick = rtick;
		l_tick = ltick;
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

	//rt_queue_create(&rqueue, "rQueue", QUEUE_SIZE, 40, Q_FIFO);
        //rt_queue_create(&lqueue, "lQueue", QUEUE_SIZE, 40, Q_FIFO);
	
	rt_sem_create(&rSem, "rSem", 1, S_FIFO);
        rt_sem_create(&lSem, "lSem", 1, S_FIFO);

	rt_task_create(&rEncoder_task, "rEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&rEncoder_task, &rEncoder, 0);

        //&task, name, stack size (0 - default), priority, mode 
        rt_task_create(&lEncoder_task, "lEnc Task", 0, 50, 0);
        //&task, task function, function argument
        rt_task_start(&lEncoder_task, &lEncoder, 0);

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
	rt_task_delete(&rEncoder_task);
        rt_task_delete(&lEncoder_task);
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
