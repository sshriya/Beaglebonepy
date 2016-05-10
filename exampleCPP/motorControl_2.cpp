/* Control four motors on land rover 5

M1 - P9_23, P9_27 : PWM P9_22
M2 - p8_8, P8_10 : PWM P9_42
M3 - P8_12, P8_14 : PWM P9_16
M4 - P8_16, P8_18 : PWM P9_14
*/

#include<iostream>
#include<unistd.h> //for usleep
#include"GPIO.h"
//#include"PWM.h"      //placed in the current directory for ease of use

using namespace exploringBB;
using namespace std;

//void moveForward(void);

int main(){
   //Select logic pins
   GPIO outGPIO11(49), outGPIO12(115);
   GPIO outGPIO21(67), outGPIO22(68);
   GPIO outGPIO31(44), outGPIO32(26);
   GPIO outGPIO41(46), outGPIO42(65);

   // Set direction for logic pins
   outGPIO11.setDirection(OUTPUT);
   outGPIO12.setDirection(OUTPUT);
   outGPIO21.setDirection(OUTPUT);
   outGPIO22.setDirection(OUTPUT);
   outGPIO31.setDirection(OUTPUT);
   outGPIO32.setDirection(OUTPUT);
   outGPIO41.setDirection(OUTPUT);
   outGPIO42.setDirection(OUTPUT);

   //Select pwm pins
  /* PWM pwm_m1("pwm_test_P9_22.16");  
   PWM pwm_m2("pwm_test_P9_42.17");  
   PWM pwm_m3("pwm_test_P9_16.19");  
   PWM pwm_m4("pwm_test_P9_14.18");  
  
   //Configure pwm pins
   pwm_m1.setPeriod(100000);         // Set the period in ns
   pwm_m1.setDutyCycle(100.0f);       // duty cycle:percentage, 0-off
   pwm_m1.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm_m1.run();   
*/
  for (int i=1; i<10000; i++){
	//Motor 1
      outGPIO11.setValue(HIGH);
      outGPIO12.setValue(LOW);
  /*    pwm_m1.setDutyCycle(25.0f);

        //Motor 2
      outGPIO21.setValue(HIGH);
      outGPIO22.setValue(LOW);
      pwm_m2.setDutyCycle(25.0f);

        //Motor 3
      outGPIO31.setValue(HIGH);
      outGPIO32.setValue(LOW);
      pwm_m3.setDutyCycle(25.0f);

        //Motor 4
      outGPIO41.setValue(HIGH);
      outGPIO42.setValue(LOW);
      pwm_m4.setDutyCycle(25.0f);
*/

	//sleep(2); //sleep for 2 seconds
	}
	return 0;
 }


