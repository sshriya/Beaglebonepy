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

void demo(void *arg)
{
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;

	rt_printf("Hello World!\n");
	//curtask = rt_task_self();
	//rt_task_inquire(curtask, &curtaskinfo);
	//rt_printf("Task name: %s \n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
	char str[10];
	int err;

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_printf("start task\n");
	sprintf(str, "hello");
	//rt_task_create(&demo_task, str, 0, 50, 0);
	//rt_task_start(&demo_task, &demo, 0);
	err = rt_task_spawn(&demo_task, str, TASK_STKSZ, TASK_PRIO, TASK_MODE, &demo, NULL);
	if(!err)rt_task_delete(&demo_task);
	printf("task finished\n");
	return 0;
}
