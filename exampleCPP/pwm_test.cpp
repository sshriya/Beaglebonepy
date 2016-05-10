/*
 * pwm_test.cpp  Created on: 3rd Feb 2015
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring BeagleBone"
 * See: www.exploringbeaglebone.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European
 * Union Public License (the "EUPL") version 1.1 as published by the
 * European Union. Any use of this Software, other than as authorized
 * under this License is strictly prohibited (to the extent such use
 * is covered by a right of the copyright holder of this Software).
 *
 * This Software is provided under the License on an "AS IS" basis and
 * without warranties of any kind concerning the Software, including
 * without limitation merchantability, fitness for a particular purpose,
 * absence of defects or errors, accuracy, and non-infringement of
 * intellectual property rights other than copyright. This disclaimer
 * of warranty is an essential part of the License and a condition for
 * the grant of any rights to this Software.
 *
 * For more details, see http://www.derekmolloy.ie/
 */

#include<iostream>
#include"PWM.h"      //placed in the current directory for ease of use

using namespace exploringBB;
using namespace std;

int main(){
   PWM pwm_m1("pwm_test_P9_22.16");  // example alternative pin
   PWM pwm_m2("pwm_test_P9_42.17");  // P9_42 MUST be loaded as a slot before use
   PWM pwm_m3("pwm_test_P9_16.19");  // example alternative pin
   PWM pwm_m4("pwm_test_P9_14.18");  // P9_42 MUST be loaded as a slot before use
  
   pwm_m1.setPeriod(100000);         // Set the period in ns
   pwm_m1.setDutyCycle(20.0f);       // Set the duty cycle as a percentage
   pwm_m1.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm_m1.run();                     // start the PWM output
   pwm_m2.setPeriod(100000);         // Set the period in ns
   pwm_m2.setDutyCycle(40.0f);       // Set the duty cycle as a percentage
   pwm_m2.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm_m2.run(); 
   pwm_m3.setPeriod(100000);         // Set the period in ns
   pwm_m3.setDutyCycle(60.0f);       // Set the duty cycle as a percentage
   pwm_m3.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm_m3.run(); 
   pwm_m4.setPeriod(100000);         // Set the period in ns
   pwm_m4.setDutyCycle(80.0f);       // Set the duty cycle as a percentage
   pwm_m4.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm_m4.run(); 


}
