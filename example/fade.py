## ------Demonstrated use of PWM signal on P9_14-----------
# Shriya Shah

from Adafruit_BBIO import PWM
import time

led = "P9_14"

# PWM.start(channel, duty, freq = 2000, polarity=0)
PWM.start(led, 0) # start with 0 duty cycle

try:
	while True:
		# keep incleasing brightness of led by increasing the 
		# duty cycle
		for level in range(0, 100):
			# Set the duty cycle
			PWM.set_duty_cycle(led, level)
 			time.sleep(0.01)
		# Turn off LEd by setting duty cycle to 0
		PWM.set_duty_cycle(led, 0)

except KeyboardInterrupt:
	PWM.cleanup()
