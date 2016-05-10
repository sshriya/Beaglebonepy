#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

RT_TASK demo_task1, demo_task2;

#define TASK_PRIO1 20
#define TASK_PRIO2 99
#define TASK_MODE 0
#define TASK_STKSZ 0
#define TASK_PERIOD1 5000000000
#define TASK_PERIOD2 2500000000
void periodic_task1(void *arg)
{
	RTIME now, previous;
	previous = rt_timer_read();
	for(;;){
		rt_task_wait_period(NULL);
		now = rt_timer_read();
		rt_printf("Task 1, Time taken: %ld.%04ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
		previous = now;
	}
}

void periodic_task2(void *arg)
{
        RTIME now, previous;
        previous = rt_timer_read();
        for(;;){
                rt_task_wait_period(NULL);
                now = rt_timer_read();
                rt_printf("Task 2, Time taken: %ld.%04ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
                previous = now;
        }
}

int main(int argc, char* argv[])
{
	int e1, e2, e3, e4, e5, e6, e7;
	RT_TIMER_INFO info;

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	
	e1 = rt_timer_set_mode(TM_ONESHOT);
	e2 = rt_task_create(&demo_task1, "periodic task 1", TASK_STKSZ, TASK_PRIO1, TASK_MODE);
	e3 = rt_task_set_periodic(&demo_task1, TM_NOW, rt_timer_ns2ticks(TASK_PERIOD1));
        e4 = rt_task_create(&demo_task2, "periodic task 2", TASK_STKSZ, TASK_PRIO2, TASK_MODE);
        e5 = rt_task_set_periodic(&demo_task2, TM_NOW, rt_timer_ns2ticks(TASK_PERIOD2));
	
	//e6 = rt_task_start(&demo_task1, &periodic_task1, NULL);
        e7 = rt_task_start(&demo_task2, &periodic_task2, NULL);
	e6 = rt_task_start(&demo_task1, &periodic_task1, NULL);

	if(e1|e2|e3|e6){
		rt_printf("Error!!\n");
		rt_task_delete(&demo_task1);
		rt_task_delete(&demo_task2);
		exit(1);
	}
	printf("Press any key to end..\n");
	getchar();
	rt_task_delete(&demo_task1);
	rt_task_delete(&demo_task2);
	return 0;
}
