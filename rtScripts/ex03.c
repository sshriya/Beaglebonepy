/*
 This exampe shows how to launch multiple tasks  where some tasks are periodic while other
are one-shot.
*/

// Shriya Shah, 20th April, 2016
 
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <signal.h>

#define period1 1000000
#define period2 2000000
#define period3 3000000

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

//	rt_printf("In Task 1!\n");
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

  //      rt_printf("In Task 2!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
	rt_printf("Task name: %s \n", curtaskinfo.name);
	//rt_printf("argument passed is: %d\n", *arg);
}

//Periodic task
void demo3(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	int i = 0;
	int a = 0;
	RTIME now, previous;	
	rt_task_set_periodic(NULL, TM_NOW, period1);
        //rt_printf("In Task 3!\n");
	previous = rt_timer_read();
	a = 0;
	while (1){
                rt_task_wait_period(NULL);
		for(i = 0; i < 1000; i++)
			a = a+1;
		rt_printf("task 3\n");
		//now = rt_timer_read();
			//rt_printf("in periodic task 3, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
		//previous = now;
	}
	//rt_printf("task 3\n");

}
void demo4(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	int i = 0;
	int a = 0;
	RTIME now, previous;       
  //      rt_printf("In Task 4!\n");
        rt_task_set_periodic(NULL, TM_NOW, period2);
 	previous = rt_timer_read();

        while (1){
                rt_task_wait_period(NULL);
                //now = rt_timer_read();
                //rt_printf("in periodic task 4, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
                //previous = now;
		a = 0;
		for(i=0; i<1000; i++)
			//rt_printf("task 4\n");
			a = a+1;
		rt_printf("task 4\n");
        }
	rt_printf("task 4\n");

}

void demo5(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
	RTIME now, previous;
	int i = 0;
	int a = 0;
        rt_task_set_periodic(NULL, TM_NOW, period3);
    //    rt_printf("In Task 5!\n");
	previous = rt_timer_read();
        while (1){
                rt_task_wait_period(NULL);
                //now = rt_timer_read();
                //rt_printf("in periodic task 5, time taken:%ld. %06ld ms\n", (long)(now-previous)/1000000, (long)(now-previous)%1000000);
                //previous = now;
		a = 0;
		for(i = 0; i<1000; i++)
			a = a+1;
			rt_printf("Task 5\n");

        }
	rt_printf("task 5\n");


}


int main(int argc, char* argv[])
{
	char str[10];
	int i = 1;
	int j = 2;
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_printf("start task\n");
	/*
	sprintf(str, "task 1");
	//&task, name, stack size (0 - default), priority, mode 
	rt_task_create(&demo_task1, str, 0, 50, 0);
	//&task, task function, function argument
	rt_task_start(&demo_task1, &demo1, 0);

        sprintf(str, "task 2");
        rt_task_create(&demo_task2, str, 0, 20, 0);
        rt_task_start(&demo_task2, &demo2, 0);
*/
        sprintf(str, "task 3");
        rt_task_create(&demo_task3, str, 0, 10, 0);
        rt_task_start(&demo_task3, &demo3, 0);

        sprintf(str, "task 4");
        rt_task_create(&demo_task4, str, 0, 90, 0);
        rt_task_start(&demo_task4, &demo4, 0);

        sprintf(str, "task 5");
        rt_task_create(&demo_task5, str, 0, 50, 0);
        rt_task_start(&demo_task5, &demo5, 0);

	rt_printf("end program : ctrl+c\n");
	pause();

}
