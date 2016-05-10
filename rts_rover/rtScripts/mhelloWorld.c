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

void task1(void *arg)
{
	rt_printf("Task 1!\n");
}

void task2(void *arg)
{
        rt_printf("Task 2!\n");
}

int main(int argc, char* argv[])
{
	int err1, err2;

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_printf("start task\n");
	
	err1 = rt_task_spawn(&demo_task1, "task 1", TASK_STKSZ, TASK_PRIO1, TASK_MODE, &task1, NULL);
	err2 = rt_task_spawn(&demo_task2, "task 2", TASK_STKSZ, TASK_PRIO2, TASK_MODE, &task2, NULL);
	if(!(err1 | err2)){
		rt_task_delete(&demo_task1);
		rt_task_delete(&demo_task2);
	}
	printf("task finished\n");
	return 0;
}
