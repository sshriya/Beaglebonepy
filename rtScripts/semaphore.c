/*

Use semaphore for synchronization - global variables


*/

// Shriya Shah, 26th April, 2016
 
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <native/sem.h>
#include <signal.h>

#define ITER 10

static RT_TASK demo_task1;
static RT_TASK demo_task2;
RT_TASK demo_task3;
RT_TASK demo_task4;
RT_TASK demo_task5;

RT_SEM semGlobal;

int global = 0;

void demo1(void *arg)
{
	int i;
	for(i=0; i<ITER;i++){
		rt_sem_p(&semGlobal, 0);
		rt_printf("in Task 1 and global is = %d.....\n", ++global);
		rt_sem_v(&semGlobal);
	}
}

void demo2(void *arg){
	int i;
        for(i=0; i<ITER;i++){
		rt_sem_p(&semGlobal, 0);
                rt_printf("in Task 2 and global is = %d.....\n", --global);
		rt_sem_v(&semGlobal);
		}
}
/*
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

*/
int main(int argc, char* argv[])
{
	char str[10];
	int i = 1;
	int j = 2;
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_printf("start task\n");
	
	rt_sem_create(&semGlobal, "globalSem", 1, S_FIFO);
	sprintf(str, "task 1");
	//&task, name, stack size (0 - default), priority, mode 
	rt_task_create(&demo_task1, str, 0, 1, 0);
	//&task, task function, function argument
	//rt_task_start(&demo_task1, &demo1, 0);
/*
        sprintf(str, "task 3");
        rt_task_create(&demo_task3, str, 0, 30, 0);
        //rt_task_start(&demo_task3, &demo3, 0);

        sprintf(str, "task 4");
        rt_task_create(&demo_task4, str, 0, 40, 0);
        //rt_task_start(&demo_task4, &demo4, 0);

        sprintf(str, "task 5");
        rt_task_create(&demo_task5, str, 0, 50, 0);
        //rt_task_start(&demo_task5, &demo5, 0);
*/
        sprintf(str, "task 2");
        rt_task_create(&demo_task2, str, 0, 1, 0);
      
	  rt_task_start(&demo_task1, &demo1, 0);
          rt_task_start(&demo_task2, &demo2, 0);
//        rt_task_start(&demo_task3, &demo3, 0);
  //      rt_task_start(&demo_task4, &demo4, 0);
    //    rt_task_start(&demo_task5, &demo5, 0);

        //rt_task_start(&demo_task1, &demo1, 0);
	return 0;
}
