# -----Demonstrate use of digital Input -------
# Connect toggle button to P9_12

# Shriya Shah

from Adafruit_BBIO import GPIO
import time

input_pin = "P9_12"

GPIO.setup(input_pin, GPIO.IN)

try:
	while True:
		val = GPIO.input(input_pin)
		print "Switch state is: ", val
		time.sleep(0.25)
except KeyboardInterrupt:
	GPIO.cleanup()	
