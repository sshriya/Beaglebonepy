# --- Demonstrate use of ADC AIN6 - P9_35 -----
# Shriya Shah

from Adafruit_BBIO import ADC

ADC.setup()

adc_pin = "P9_35"

try:
	while True:
		adc_in = ADC.read(adc_pin)
		adc_value = 1.8*adc_in
		print "adc value = {:0.02f}".format(adc_value)

except KeyboardInterrupt:
	pass
