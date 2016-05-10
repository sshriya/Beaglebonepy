/* Control a DC motor:
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

#define period1 100000000
#define period2 2000000000

RT_TASK rMotor_task;
RT_TASK lMotor_task;


void rMotor(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	RTIME now, previous;
	int i = 0;
	int fd = open("/dev/mem",O_RDWR | O_SYNC);
    	ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);	
	pinconf1[OE_ADDR/4] &= pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1 << 28)|(1<<16)|(1<<17)|(1<<5)|(1<<4)|(1<<1))); 
	//configure logic pins
     	pinconf1[GPIO_DATAOUT/4]  |= (1 << 28); //set pin  P9_12
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 17); // clear pin P9_15
	rt_task_set_periodic(NULL, TM_NOW, period1);
        rt_printf("Controling Right Motors!\n");
	//previous = rt_timer_read();
	
	for (;;){
		rt_printf("running task\n");
                rt_task_wait_period(NULL);
		//now = rt_timer_read();
		previous = rt_timer_read();
		for(i=0; i<100000; i++){
			pinconf1[GPIO_DATAOUT/4] |= (1 << 17); //PWM on pin P9_23
			//rt_printf("in loop\n");
		//rt_task_sleep(period1);
		//pinconf1[GPIO_DATAOUT/4] ^= (1 << 17); //toggle pin
			}
		now = rt_timer_read();
		rt_printf("right Motor PWM, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
		previous = now;
		pinconf1[GPIO_DATAOUT/4] ^= (1 << 17); //Off
	}
}

void lMotor(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        RTIME now, previous;
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
        ulong* pinconf2 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_ADDR);     
        pinconf2[OE_ADDR/4] &= pinconf2[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1<<2)|(1 << 3)|(1<<5))); 
        //configure logic pins
        pinconf2[GPIO_DATAOUT/4]  |= (1 << 2); //set pin  P8_7
     	pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 3); // clear pin P8_8
        rt_task_set_periodic(NULL, TM_NOW, period2);
        rt_printf("Controling Left Motors!\n");
        previous = rt_timer_read();
        while (1){
                rt_task_wait_period(NULL);
                now = rt_timer_read();
                pinconf2[GPIO_DATAOUT/4] |= (1 << 5); //PWM on pin P8_9
                rt_task_sleep(period1);
                pinconf2[GPIO_DATAOUT/4] ^= (1 << 5); //toggle pin
                rt_printf("Left Motor PWM, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
                previous = now;
        }
}

int main(){
    	char str[10];
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_printf("start task\n");
	
	sprintf(str, "rMotor");
	//&task, name, stack size (0 - default), priority, mode 
	rt_task_create(&rMotor_task, str, 0, 50, 0);
	//&task, task function, function argument
	rt_task_start(&rMotor_task, &rMotor, 0);

        sprintf(str, "lMotor"); 
        //rt_task_create(&lMotor_task, str, 0, 50, 0);
        //rt_task_start(&lMotor_task, &lMotor, 0);

	rt_printf("end program : ctrl+c\n");
	pause();

	rt_task_delete(&rMotor_task);
}
