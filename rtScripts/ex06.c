/* Preemptive Priority based scheduling
*/

// Shriya Shah, 22nd April, 2016
 
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/queue.h>
#include <rtdk.h>
#include <signal.h>
#include <sys/io.h>

#define NTASKS 3

#define HIGH 52
#define MID 51
#define LOW 50

RT_TASK demo_task[NTASKS];
RT_SEM mysync;

//in ns
#define BASEPERIOD 0

#define SPINTIME 1e7
#define EXECTIME 2e8

void demo(void *arg)
{
	RTIME starttime, runtime;
	int num =* (int *)arg;
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;

	rt_printf("Task: %d\n", num);

	rt_sem_p(&mysync, TM_INFINITE);
	
	runtime = 0;

	while(runtime < EXECTIME){
		rt_timer_spin(SPINTIME); //spin wait CPU
		runtime = runtime + SPINTIME;

		rt_printf("Running task: %d at ms: %d\n", num, runtime/1000000);
	}
	rt_printf("End Task: %d\n", num);
       
}

void init_xenomai(){
        mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);  
}

void startup(){

	int i;
	char str[10];

	// semaphore to sync task startup on
	rt_sem_create(&mysync, "My semaphore", 0, S_FIFO);

	rt_timer_set_mode(BASEPERIOD);//set timer to tick in nsec

	for (i=0; i < NTASKS; i++){    
        	rt_printf("start task : %d\n", i);
		sprintf(str, "task%d", i);
		//&task, name, stack size (0 - default), priority, mode 
        	rt_task_create(&demo_task[i], str, 0, 50, 0);
        	//&task, task function, function argument
        	rt_task_start(&demo_task[i], &demo, &i);
	}

	//assign priorities to tasks
	rt_task_set_priority(&demo_task[0], LOW);
	rt_task_set_priority(&demo_task[1], MID);
	rt_task_set_priority(&demo_task[2], HIGH);

	rt_printf("wake up all tasks\n");
	rt_sem_broadcast(&mysync);
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
	rt_task_delete(&demo_task[1]);
}


int main(int argc, char* argv[])
{
	printf("\n Press Ctrl+c to quit\n\n");

	init_xenomai();

	startup();

	//wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
