/* Control a DC motor:
Motor 1 & 3: Right 
Logic 2 - P9_12 - GPIO1[28]
Logic 1 - P9_15 - GPIO1[16]
PWM 1 - P9_23 - GPIO1[17]

Motor 2 & 4: Left
Logic 1 - P8_7 - GPIO2[2]
Logic 2 - P8_8 - GPIO2[3]
PWM 1 - P8_9 - GPIO2[5]

*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

int main(){
    int i = 0;
    int fd = open("/dev/mem",O_RDWR | O_SYNC);
    ulong* pinconf1 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_ADDR);
    ulong* pinconf2 =  (ulong*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_ADDR);
    //configure motor pins as output
    pinconf1[OE_ADDR/4] &=  pinconf1[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1 << 28)|(1<<16)|(1<<17)|(1<<5)|(1<<4)|(1<<1)));    
    pinconf2[OE_ADDR/4] &=  pinconf2[OE_ADDR/4] &= (0xFFFFFFFF ^ ((1<<2)|(1 << 3)|(1<<5))); 
    
   //configure logic pins
     pinconf1[GPIO_DATAOUT/4]  |= (1 << 28); //set pin  P9_12
     pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 17); // clear pin P9_15
     pinconf2[GPIO_DATAOUT/4]  |= (1 << 2); //set pin  P8_7
     pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 3); // clear pin P8_8

   //generate PWM
     for(i = 0 ; i < 2; i++){
     	pinconf1[GPIO_DATAOUT/4]  |= (1 << 17); //PWM on pin P9_23
     	pinconf2[GPIO_DATAOUT/4]  |= (1 << 5); //PWM on pin P8_9
     	sleep(1);
     	pinconf1[GPIO_DATAOUT/4]  &= ~(1 << 17); //toggle pin
     	pinconf2[GPIO_DATAOUT/4]  &= ~(1 << 5); //toggle pin
     	sleep(1);
    }
    return 0;
}
