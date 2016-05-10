
#include<iostream>
#include<unistd.h> //for usleep
#include"GPIO.h"
using namespace exploringBB;
using namespace std;

int main(){
   GPIO outGPIO(49);

   // Basic Output - Flash the LED 10 times, once per second
   outGPIO.setDirection(OUTPUT);
   for (int i=0; i<100000; i++){
      outGPIO.setValue(HIGH);
      usleep(300); //micro-second sleep 0.5 seconds
      outGPIO.setValue(LOW);
      usleep(300);
   }
   /*
   // Fast write to GPIO 1 million times
   outGPIO.streamOpen();
   for (int i=0; i<1000000; i++){
      outGPIO.streamWrite(HIGH);
      outGPIO.streamWrite(LOW);
   }
   outGPIO.streamClose();
*/
   return 0;
}
