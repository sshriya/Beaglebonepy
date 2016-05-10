/* Slots to load:
Right motor PWM - P9_42
echo bone_pwm_P9_42 > $SLOTS
echo am33xx_pwm > $SLOTS

Left Motor PWM - P9_22
echo bone_pwm_P9_22 > $SLOTS

*/


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

#define period_motorStop 500000 //.1 ms
#define period 200000000 //200 ms

RT_TASK rMotor_task;
RT_TASK lMotor_task;
RT_TASK rMotor_stop_task;
RT_TASK lMotor_stop_task;
int flagStop = 0;
int duty = 0;

void rMotor_stop(void *arg)
{
//	rt_task_delete(&rMotor_task);
        int fd;
        char buf[MAX_BUF];
	RTIME now, previous;
	long MAX = 0;
	int flag = 1;

        //Turn off
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_42.12/duty"); 
        rt_task_set_periodic(NULL, TM_NOW, period_motorStop);
        rt_printf("Running right motor STOP task!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
		if(flagStop){
 		        rt_task_delete(&rMotor_task);
	       		fd = open(buf, O_RDWR); 
        		if(fd < 0){
            			perror("Problem opening Duty file - right motor");
         		}
        		//Duty 0 to stop
        		write(fd, "0", 5); 
        		close(fd);
			rt_printf("RMotor stopped \n");
			now = rt_timer_read();
			if (flag){
				MAX = now- previous;
				flag = 0;
			}

			if((long)((now - previous)) > MAX){
				MAX = (long)((now - previous)) ;
			}
			rt_printf("WCET Right Motor Stop: %ld \n", MAX);
		}
	}
}

void lMotor_stop(void *arg)
{
//      rt_task_delete(&rMotor_task);
        int fd;
        char buf[MAX_BUF];
        RTIME now, previous;
        long MAX = 0;
        int flag = 1;

        //Turn off
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_22.13/duty"); 
        rt_task_set_periodic(NULL, TM_NOW, period_motorStop);
        rt_printf("Running left motor STOP task!\n");

        while (1){
                rt_task_wait_period(NULL);
                previous = rt_timer_read();
                if(flagStop){
		      rt_task_delete(&lMotor_task);
                        fd = open(buf, O_RDWR); 
                        if(fd < 0){
                                perror("Problem opening Duty file - leeft motor");
                        }
                        //Duty 0 to stop
                        write(fd, "0", 5); 
                        close(fd);
                        rt_printf("LMotor stopped \n");
                        now = rt_timer_read();
                        if (flag){
                                MAX = now- previous;
                                flag = 0;
                        }

                        if((long)((now - previous)) > MAX){
                                MAX = (long)((now - previous)) ;
                        }
                        rt_printf("WCET Left Motor Stop: %ld \n", MAX);
                }

	}
}


void rMotor(void *arg)
{
	int fd, per;
        char buf[MAX_BUF];
        char duty_cycle[14];
	char Period[14]; 
	RTIME now, previous;
        long MAX = 0;
	int flag = 1;

       //Set polarity
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_42.12/polarity"); 
        fd = open(buf, O_RDWR); 
        if(fd < 0){
            perror("Problem opening Polarity file - right motor");
         }
	//polarity 0: 0 Duty - 0V
        write(fd, "0", 5); 
        close(fd);
	
	//Set period
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_42.12/period"); 
	per = 10000;
        sprintf(Period,"%d",per);
        fd = open(buf, O_RDWR); 
        if(fd < 0){
            perror("Problem opening Period file - right motor");
         }
	write(fd, &Period, 10); 
        close(fd);

	// set duty cycle in the periodc task
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_42.12/duty");

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Running right motor task!\n");

        while (1){
                rt_task_wait_period(NULL);
		previous = rt_timer_read();
	
		// set duty cycle
                sprintf(duty_cycle,"%d",duty);
		fd = open(buf, O_RDWR); //Open ADC as read only
        	if(fd < 0){
                	perror("Problem opening Duty file - right motor");
        	}
        	write(fd, &duty_cycle, 10); //read upto 4 digits 0-1799
        	close(fd);

		if (flag){
                        MAX = now- previous;
                        flag = 0;
                }

		if((long)((now - previous)) > MAX){
                        MAX = (long)((now - previous)) ;
                }
                rt_printf("WCET Right Motor: %ld \n", MAX);
	}
	return;
} 

void lMotor(void *arg)
{
        int fd, per;
        char buf[MAX_BUF];
        char duty_cycle[14];
        char Period[14]; 
        RTIME now, previous;
        long MAX = 0;
	int flag = 1;

       //Set polarity
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_22.13/polarity"); 
        fd = open(buf, O_RDWR); 
        if(fd < 0){
            perror("Problem opening Polarity file - left motor");
         }
        //polarity 0: 0 Duty - 0V
        write(fd, "0", 5); 
        close(fd);

        //Set period
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_22.13/period"); 
        per = 10000;
        sprintf(Period,"%d",per);
        fd = open(buf, O_RDWR); 
        if(fd < 0){
            perror("Problem opening Period file - left motor");
         }
        write(fd, &Period, 10); 
        close(fd);

        // set duty cycle in the periodc task
        snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/pwm_test_P9_22.13/duty");

        rt_task_set_periodic(NULL, TM_NOW, period);
        rt_printf("Running Left motor task!\n");

        while (1){
                rt_task_wait_period(NULL);
                previous = rt_timer_read();

                // set duty cycle
                sprintf(duty_cycle,"%d",duty);
                fd = open(buf, O_RDWR); //Open ADC as read only
                if(fd < 0){
                        perror("Problem opening Duty file - left motor");
                }
                write(fd, &duty_cycle, 10); //read upto 4 digits 0-1799
                close(fd);


                if (flag){
                        MAX = now- previous;
                        flag = 0;
                }

                if((long)((now - previous)) > MAX){
                        MAX = (long)((now - previous)) ;
                }
                rt_printf("WCET left Motor: %ld \n", MAX);

        }
        return;
} 

 
void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){ 
//        rt_task_create(&rMotor_task, "rMotor Task", 0, 50, 0);
//        rt_task_create(&lMotor_task, "lMotor Task", 0, 50, 0);
        //rt_task_create(&rMotor_stop_task, "rMotor Stop Task", 0, 51, 0);
        //rt_task_create(&lMotor_stop_task, "lMotor Stop Task", 0, 51, 0);
        rt_task_create(&rMotor_task, "rMotor Task", 0, 50, 0);
        rt_task_create(&lMotor_task, "lMotor Task", 0, 50, 0);

       // rt_task_start(&rMotor_stop_task, &rMotor_stop, 0);
        //rt_task_start(&lMotor_stop_task, &lMotor_stop, 0);
        rt_task_start(&rMotor_task, &rMotor, 0);
        rt_task_start(&lMotor_task, &lMotor, 0);
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

void do_nothing(){
int i ;
for (i = 0; i < 10000; i++);
}

void cleanup(){

	rt_printf("deleting motor tasks\n");
        rt_task_delete(&rMotor_task);
        rt_task_delete(&lMotor_task);

//	rt_printf("Starting motor stop tasks \n");
//        rt_task_start(&rMotor_stop_task, &rMotor_stop, 0);
//        rt_task_start(&lMotor_stop_task, &lMotor_stop, 0);
	
//	do_nothing();

//	rt_task_delete(&rMotor_task);
//        rt_task_delete(&lMotor_task);
	rt_printf("deleteing motor stop tasks \n");
        rt_task_delete(&rMotor_stop_task);
        rt_task_delete(&lMotor_stop_task);

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
	flagStop = 1;
	cleanup();

	printf("\n Ending program!\n\n");

}
