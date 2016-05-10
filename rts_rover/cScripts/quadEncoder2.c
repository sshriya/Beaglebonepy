/*
 Read an input pin using memory map
Encoder r - A: P8_11
	    B: P8_12
Encoder l - A: P8_15
	    B: P8_16
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <stdio.h>
#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

#define HIGH 1
#define LOW 0
using namespace std;

//bool read_rEncoder(*, char);

int main(){

    int rtick = 0;
    bool rBSet;
    int r_last = LOW;
    int inr = 0;
    int inrB = 0;

    int fd = open("/dev/mem",O_RDWR | O_SYNC);
    ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
    //configure encoder pins as input
    pinconf1[OE_ADDR/4]  |= ((1<<13)|(1 << 12)|(1<<15)|(1<<14)); //P8_11, P8_12, P8_15, P8_16   

   //Read
	for (;;){
		//inr = read_rEncoder(*pinconf1, logicA); //logic A
		if(pinconf1[GPIO_DATAIN/4] & (1 << 13)){
//                        cout << "A is HIGH" <<endl;
                        inr = HIGH;
                }else{
//                        cout << "A is LOW" << endl;
                        inr = LOW;
                }


		if(((r_last == LOW)&&(inr == HIGH)) || ((r_last == HIGH)&&(inr == LOW))){
				rtick++;
			}
		r_last = inr;
		cout << rtick << endl;
//		sleep(.01);
	}
}

