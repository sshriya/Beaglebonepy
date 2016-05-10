#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

RT_TASK demo_task;

#define TASK_PRIO 20
#define TASK_MODE 0
#define TASK_STKSZ 0
#define TASK_PERIOD 5000000000

void periodic_task(void *arg)
{
	RTIME now, previous;
	previous = rt_timer_read();
	for(;;){
		rt_task_wait_period(NULL);
		now = rt_timer_read();
		rt_printf("Time taken: %ld.%04ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
		previous = now;
	}
}

int main(int argc, char* argv[])
{
	int e1, e2, e3, e4;
	RT_TIMER_INFO info;

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	
	e1 = rt_timer_set_mode(TM_ONESHOT);
	e2 = rt_task_create(&demo_task, "periodic task", TASK_STKSZ, TASK_PRIO, TASK_MODE);
	e3 = rt_task_set_periodic(&demo_task, TM_NOW, rt_timer_ns2ticks(TASK_PERIOD));
	e4 = rt_task_start(&demo_task, &periodic_task, NULL);

	if(e1|e2|e3|e4){
		rt_printf("Error!!\n");
		rt_task_delete(&demo_task);
		exit(1);
	}
	printf("Press any key to end..\n");
	getchar();
	rt_task_delete(&demo_task);
	return 0;
}
