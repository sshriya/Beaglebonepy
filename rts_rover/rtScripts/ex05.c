/* using intertask communication  - Message Queue which is a buffer managed by the OS

Application : Pass messages between two tasks and print them.
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

#define NTASKS 2
#define QUEUE_SIZE 255
#define MAX_MESSAGE_LENGTH 40

RT_TASK task_struct[NTASKS];

RT_QUEUE myqueue;

void taskOne(void *arg)
{
	int retval;
	char message[] = "From TAsk 1";

	//Send Message
	//queue_write - RT_QUEUE, buffer, size, mode: Q_FIFO, Q_NORMAL, Q_PRIO
	retval = rt_queue_write(&myqueue, message, sizeof(message), Q_NORMAL);

	if (retval < 0){
		rt_printf("Sending error\n");
	}else{
		rt_printf("task 1 sent message to mailbox\n");
	}
}

void taskTwo(void *arg)
{
        int retval;
        char msgBuf[MAX_MESSAGE_LENGTH];

        //Recieve Message 
        //queue_write - RT_QUEUE, buffer, size, mode: Q_FIFO, Q_NORMAL, Q_PRIO
        retval = rt_queue_read(&myqueue, msgBuf, sizeof(msgBuf), TM_INFINITE);

        if (retval < 0){
                rt_printf("Recieving error\n");
        }else{
                rt_printf("task 2 recieved message from mailbox: %s\n", msgBuf);
		rt_printf("with size %d\n", retval);
        }
       
}

void init_xenomai(){
	rt_print_auto_init(1);
        mlockall(MCL_CURRENT|MCL_FUTURE);
}

void startup(){

	int i;
	char str[10];

	void (*task_func[NTASKS]) (void *arg);
	task_func[0] = taskOne;
	task_func[1] = taskTwo;

	rt_queue_create(&myqueue, "myqueue", QUEUE_SIZE, 10, Q_FIFO);
	rt_timer_set_mode(0);//set timer to tick in nsec

	for (i=0; i < NTASKS;i++){    
        	rt_printf("start task : %d\n", i);
		sprintf(str, "task%d", i);
		//&task, name, stack size (0 - default), priority, mode 
        	rt_task_create(&task_struct[i], str, 0, 50, 0);
        	//&task, task function, function argument
        	rt_task_start(&task_struct[i], task_func[i], &i);
	}
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
	rt_task_delete(&task_struct[1]);
}


int main(int argc, char* argv[])
{
	printf("\n Press Ctrl+c to quit\n\n");

	init_xenomai();

	startup();

	wait_for_ctrl_c();

	cleanup();

	printf("\n Ending program!\n\n");

}
