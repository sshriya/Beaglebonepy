/*
 This exampe shows how to launch multiple tasks in Xenomai. Arguments can be passed as the last parameter in the 
rt_task_start() function. 

Observation - chaging the priority does not change the order in which tasks are started. It might be beacuse a 
simple FIFO scheduler has been used.
*/

// Shriya Shah, 20th April, 2016
 
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

RT_TASK demo_task1;
RT_TASK demo_task2;
RT_TASK demo_task3;
RT_TASK demo_task4;
RT_TASK demo_task5;

void demo1(void *arg)
{
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;
       // int num =  (int *)arg;

	rt_printf("In Task 1!\n");
	curtask = rt_task_self();
	rt_task_inquire(curtask, &curtaskinfo);
	rt_printf("Task name: %s \n", curtaskinfo.name);
	//rt_printf("argument passed is: %d\n", *arg);

}
void demo2(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	//int num =  (int *)arg;

        rt_printf("In Task 2!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
        rt_printf("Task name: %s \n", curtaskinfo.name);
	//rt_printf("argument passed is: %d\n", *arg);
}

void demo3(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        //int num =  (int *)arg;

        rt_printf("In Task 3!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
        rt_printf("Task name: %s \n", curtaskinfo.name);
        //rt_printf("argument passed is: %d\n", num);

}
void demo4(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        //int num =  (int *)arg;

        rt_printf("In Task 4!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
        rt_printf("Task name: %s \n", curtaskinfo.name);
        //rt_printf("argument passed is: %d\n", num);
}

void demo5(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        //int num =  (int *)arg;

        rt_printf("In Task 5!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
        rt_printf("Task name: %s \n", curtaskinfo.name);
        //rt_printf("argument passed is: %d\n", num);
}


int main(int argc, char* argv[])
{
	char str[10];
	int i = 1;
	int j = 2;
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_printf("start task\n");

	sprintf(str, "task 1");
	//&task, name, stack size (0 - default), priority, mode 
	rt_task_create(&demo_task1, str, 0, 10, 0);
	//&task, task function, function argument
	//rt_task_start(&demo_task1, &demo1, 0);

        sprintf(str, "task 3");
        rt_task_create(&demo_task3, str, 0, 30, 0);
        //rt_task_start(&demo_task3, &demo3, 0);

        sprintf(str, "task 4");
        rt_task_create(&demo_task4, str, 0, 40, 0);
        //rt_task_start(&demo_task4, &demo4, 0);

        sprintf(str, "task 5");
        rt_task_create(&demo_task5, str, 0, 50, 0);
        //rt_task_start(&demo_task5, &demo5, 0);

        sprintf(str, "task 2");
        rt_task_create(&demo_task2, str, 0, 20, 0);
        rt_task_start(&demo_task1, &demo1, 0);

        rt_task_start(&demo_task2, &demo2, 0);
        rt_task_start(&demo_task3, &demo3, 0);
        rt_task_start(&demo_task4, &demo4, 0);
        rt_task_start(&demo_task5, &demo5, 0);

        //rt_task_start(&demo_task1, &demo1, 0);

}
