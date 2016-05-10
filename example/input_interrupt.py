# ------Use of interrupts on input --------
# Switch connected to pin P9_12

from Adafruit_BBIO import GPIO
import time

input_pin = "P9_12"
GPIO.setup(input_pin, GPIO.IN)

# Add interrupt on falling edge
GPIO.add_event_detect(input_pin, GPIO.FALLING)

try:
	while True:
		# Check for interruprt in background
		if GPIO.event_detected(input_pin):
			print "Button pressed"
		time.sleep(1)

except KeyboardInterrupt:
	GPIO.cleanup()
