#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <iostream>
//#include<stdio.h>
#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

RT_TASK demo_task;

void demo(void *arg)
{
        RT_TASK *curtask;
        RT_TASK_INFO curtaskinfo;
        int fd = open("/dev/mem",O_RDWR | O_SYNC);
	int i = 0;
        ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
        pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ (1 << 28));
	rt_printf("executing task!");
    
       	pinconf1[GPIO_DATAOUT/4]  |= (1 << 28);
	rt_printf("on\n");
     	//rt_task_sleep(10000000); //in nano seconds
     	//pinconf1[GPIO_DATAOUT/4]  ^= (1 << 28);
	//rt_printf("off\n");
        //rt_task_sleep(10000000); //in nano seconds

        //rt_printf("Hello World!\n");
        curtask = rt_task_self();
        rt_task_inquire(curtask, &curtaskinfo);
        rt_printf("Task name: %s \n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
        char str[10];
        rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
        rt_printf("start task\n");
        sprintf(str, "hello");
        rt_task_create(&demo_task, str, 0, 50, 0);
        rt_task_start(&demo_task, &demo, 0);
}

