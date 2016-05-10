/*

Move the robot in two tasks for left motor and right motor
Measure encoder readings in separate tasks 
Do localization in yet another task

Motor 1 & 3: Right 
Logic 1 - P9_12 - GPIO1[28]
Logic 2 - P9_15 - GPIO1[16]
PWM 1 - P9_23 - GPIO1[17]

Motor 2 & 4: Left
Logic 1 - P8_7 - GPIO2[2]
Logic 2 - P8_8 - GPIO2[3]
PWM 1 - P8_9 - GPIO2[5]

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
#include <time.h>

#define period 100000

#define period1 800000
#define period2 800000

#define duty1 period1*0.20
#define duty2 period2*0.20

#define p1 1000000
#define p2 1000000

RT_TASK rEnc_task;
RT_TASK lEnc_task;
RT_TASK Odo_task;
RT_TASK rMotor_task;
RT_TASK lMotor_task;
RT_TASK stop_rMotor;
RT_TASK stop_lMotor;

RT_TASK dummy_task1;
RT_TASK dummy_task2;


//robot dimension
#define R 30    //Wheel Radius in mm
#define L 185   //Robot length in mm
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
	RTIME now, previous;
	long MAX = 0;
    	int fd = open("/dev/mem",O_RDWR | O_SYNC);
    	ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
    	//configure encoder pins as input
    	pinconf1[OE_ADDR/4]  |= ((1<<13)|(1 << 12)); //P8_11, P8_12, P8_15, P8_16   

        rt_task_set_periodic(NULL, TM_NOW, period);
//        rt_printf("Reading right Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
                if(pinconf1[GPIO_DATAIN/4] & (1 << 13)){
                        inr = HIGH;
                }else{
                        inr = LOW;
                }


		if(((r_last == LOW)&&(inr == HIGH)) || ((r_last == HIGH)&&(inr == LOW))){
			rtick++;
		}
		r_last = inr;
//		rt_printf("Right ticks: %d \n", rtick);
		
		//send to odometry task
		sprintf(rticks, "%d", rtick);
		rt_queue_write(&rqueue, rticks, sizeof(rticks), Q_NORMAL);
		now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET Right Encoder: %ld \n", MAX);
		
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
	RTIME now, previous;
	long MAX = 0;
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
        //configure encoder pins as input
        pinconf1[OE_ADDR/4]  |= ((1<<15)|(1<<14)); //P8_11, P8_12, P8_15, P8_16   

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Reading Left Encoder!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
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
  //              rt_printf("Left ticks: %d \n", ltick);
		
                //send to odometry task
                sprintf(lticks, "%d", ltick);
                rt_queue_write(&lqueue, lticks, sizeof(lticks), Q_NORMAL);
                now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET Left Encoder: %ld \n", MAX);


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
	RTIME now, previous;
	long MAX = 0;
        double Dr, Dc, Dl, x, y, theta, x_dt, y_dt, theta_dt, x_new, y_new, theta_new;

	Dr = Dc = Dl = 0;
	x = y = theta = 0;
	x_new=y_new=theta_new = 0;
	x_dt = y_dt = theta_dt = 0;

	rt_task_set_periodic(NULL, TM_NOW, period);	

	while(1){
		rt_task_wait_period(NULL);
		previous = rt_timer_read();
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

//		rt_printf("Robot pose (x, y, theta) is: %lf, %lf, %lf\n", x, y, theta);
		now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET Odometry: %ld \n", MAX);


	}

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
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 16); // clear pin P9_15
	rt_task_set_periodic(NULL, TM_NOW, period1);
  //      rt_printf("Controling Right Motors!\n");
	//previous = rt_timer_read();
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
//                rt_printf("Left Motor PWM, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
                now = rt_timer_read();
                if((long)((now - previous)%1000000) > MAX){
                        MAX = (long)((now - previous)%1000000) ;
                }
                rt_printf("WCET Left Motor: %ld \n", MAX);

        }
}


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
//Periodic dummy task
void dummy1(void *arg)
{	
	int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p1);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 1000; i++);
		rt_printf("Executing dummy task 1\n");
        }
}

void dummy2(void *arg)
{	
	int i = 0;
        rt_task_set_periodic(NULL, TM_NOW, p2);

        while (1){
                rt_task_wait_period(NULL);
                for(i = 0; i< 1000; i++);
                rt_printf("Executing dummy task 2\n");
        }
}

void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){

	rt_queue_create(&rqueue, "rQueue", QUEUE_SIZE, 40, Q_FIFO);
        rt_queue_create(&lqueue, "lQueue", QUEUE_SIZE, 40, Q_FIFO);

        rt_task_create(&rEnc_task, "rEnc Task", 0, 70, 0);
        rt_task_start(&rEnc_task, &rEnc, 0);
 
        rt_task_create(&lEnc_task, "lEnc Task", 0, 70, 0);
        rt_task_start(&lEnc_task, &lEnc, 0);
 
        rt_task_create(&Odo_task, "Odo Task", 0, 50, 0);
        rt_task_start(&Odo_task, &Odo, 0);

	rt_task_create(&rMotor_task, "rMotor", 0, 50, 0);
	rt_task_start(&rMotor_task, &rMotor, 0);
 
        rt_task_create(&lMotor_task, "lMotor", 0, 50, 0);
        rt_task_start(&lMotor_task, &lMotor, 0);

        rt_task_create(&stop_lMotor, "stoplMotor", 0, 99, 0);
        rt_task_create(&stop_rMotor, "stoprMotor", 0, 99, 0);

	rt_task_create(&dummy_task1, "dummy 1", 0, 0, 0);
        rt_task_start(&dummy_task1, &dummy1, 0);

        rt_task_create(&dummy_task2, "dummy 2", 0, 0, 0);
        rt_task_start(&dummy_task2, &dummy2, 0);


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

//        rt_task_start(&stop_lMotor, &stoplMotor, 0);
//        rt_task_start(&stop_rMotor, &stoprMotor, 0);

	rt_task_delete(&rEnc_task);
        rt_task_delete(&lEnc_task);
	rt_task_delete(&Odo_task);

        rt_task_start(&stop_lMotor, &stoplMotor, 0);
        rt_task_start(&stop_rMotor, &stoprMotor, 0);

	rt_task_delete(&lMotor_task);
	rt_task_delete(&rMotor_task);

	rt_task_delete(&dummy_task1);
	rt_task_delete(&dummy_task2);

	rt_task_delete(&stop_lMotor);
	rt_task_delete(&stop_rMotor);
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
//	wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
