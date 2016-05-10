#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000
using namespace std;

int main(){
    int fd = open("/dev/mem",O_RDWR | O_SYNC);
    ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
    pinconf1[OE_ADDR/4] &= ~(1 << 28);
    for(;;){
     pinconf1[GPIO_DATAOUT/4]  |= (1 << 28); //P9_12
     //pinconf1[GPIO_DATAOUT/4]  ^= (1 << 21); //User led USR0
     //sleep(1);
     pinconf1[GPIO_DATAOUT/4]  ^= (1 << 28);
     //pinconf1[GPIO_DATAOUT/4]  |= (1 << 21);
     //sleep(1);
    }

	//Turn on input mode P9_12 - GPIO1[28]
   // pinconf1[GPIO_DATAOUT/4]  &= (0xFFFFFFFF ^ ((1 << 21) | (1 << 28)));
    
	//set as input
    /*pinconf1[OE_ADDR/4] |= ( 1 << 28);
    for(int i = 0; i < 20; i++){ //poll for 20 seconds
     cout << pinconf1[GPIO_DATAIN/4]  << endl;
     if(pinconf1[GPIO_DATAIN/4] & (1  << 28)){
      cout << "on" <<endl;
      pinconf1[GPIO_DATAOUT/4]  |= (1 << 21);
     }
     else{
      cout << "off" <<endl;
      pinconf1[GPIO_DATAOUT/4] &= (0xFFFFFFFF ^ (1 << 21));
     }
     sleep(1);
    }
    pinconf1[GPIO_DATAOUT/4]  ^= (1 << 21);
    */
	return 0;
}
