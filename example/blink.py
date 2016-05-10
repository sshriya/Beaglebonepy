## ----- Blink LEd on pin P8_10 -------
# Shriya Shah

import Adafruit_BBIO.GPIO as GPIO
import time

led1 = "P8_8"

# Setup pin as output
GPIO.setup(led1, GPIO.OUT)

# Blink led in an infinite loop
try: 	
	while True:
		# Make pin high - 3.3V
		GPIO.output(led1, GPIO.HIGH)
		# Sleep for 0.5 seconds
		time.sleep(0.5)
		# Make pin Low - 0V
                GPIO.output(led1, GPIO.LOW)
                time.sleep(0.5)

except KeyboardInterrupt:
	GPIO.cleanup()

		
