# Programming Beaglebone using Python

Follow the getting started guide to setup your beaglebone. 

## Getting Started

This guide is built for debian-3.8.13. To upgrade, follow the instruction under updating the debian image. All the instructions assume that you are using Ubuntu terminal for ssh. 

### 1. Getting internet

For USB internet sharing between host computer and Beaglebone, do the following on Beaglebone side:

```
ssh 192.168.7.2 -l root
ifconfig usb0 192.168.7.2
route add default gw 192.168.7.1
```
On the host computer, do the following:

wlan0 is my internet facing interface, eth5 is the BeagleBone USB connection
```
    sudo su
    ifconfig eth5 192.168.7.1
    iptables --table nat --append POSTROUTING --out-interface wlan0 -j MASQUERADE
    iptables --append FORWARD --in-interface eth5 -j ACCEPT
    echo 1 > /proc/sys/net/ipv4/ip_forward
```
    
To check whether you are getting internet or not:
```
ping 8.8.8.8
```
If this does not work, you might need to echo nameserver to the resolv.conf file on Beaglebone:
```
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
```

### 2. Updating the image
To check your debian image:
```
uname -a
```
If your debian image is older han 3.8.13 then you need to update.

### 3. Python setup
In order to get started with programming the BeagleBone, we need to update the bone and dowload a few dependencies. 

```
sudo ntpdate pool.ntp.org
sudo apt-get update
sudo apt-get install build-essential python-dev python-setuptools python-pip python-smbus -y
sudo pip install Adafruit_BBIO
```
## GPIO
Beaglebone Black has 69 GPIO pins which can be configured as input or output. blink.py is a simple program that configures a GPIo pin as output and then toggles its state at a constant rate. To run, first connect as led between pin P8_8 and ground and then run it by: 

```
python blink.py
```

Not all pins on the Beaglebone are configured as GPIO as defalut. A single pin can have more than one functionality (SPI/ ADc/ GPIO etc). Therefore in order to use a pin which is does not work as GPIO by default, we need to use the device tree overlay. 

Device tree overlay is a simple method by which we can chooce the fuctionality of interest on a multiplexed pin. In order to check which overlay is already loaded:

```
cd /sys/devices/bone_capemgr.9
cat slots
```
This should output a list of overlays that are loaded by default. For example, the slot 5 is for HDMI cape while slot 4 is for EMMC cape. 
```
 0: 54:PF--- 
 1: 55:PF--- 
 2: 56:PF--- 
 3: 57:PF--- 
 4: ff:P-O-L Bone-LT-eMMC-2G,00A0,Texas Instrument,BB-BONE-EMMC-2G
 5: ff:P-O-L Bone-Black-HDMI,00A0,Texas Instrument,BB-BONELT-HDMI
```

The compiled overlays are present in the /lib/firmware directory on the beagle bone. Before we load an overlay, it would be useful to pul SLOTS and PINS variable in the bash profile file. This can be done as follows:

```
nano `/.profile
```

Add the following to the profile file
```
export SLOTS=/sys/devices/bone_capemgr.9/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins
```

Now source the profile
```
source ~/.profile
```

Now, the loaded capes and pins can be seen by just:

```
cat $SLOTS
cat $PINS
```
## PWM

Beaglebone has 3 PWM modules which are capable of generating 6 PWM signals. Fade.py in th examples folder shows how to use a pwm pin to control the brightness of an LED. To configure and use a PWm pin, the folowing two things are required.
```
1. Setup
PWM.start(pin, duty cycle, frequency, polarity)

2. Set duty cycle
PWM.set_duty_cycle(pin, Duty)
```

The duty cycle input can be any number between 0.0-100.0. The deafult frequency is 2000 Hz and the default polarity is 0 which means that a duty cycle value of 0 produced 0V on the pins. 
To stop the pwm pin, we can do:
```
PWM.stop(pin)
```

If suppose you want to use a pwm pin which not configured by default to be used a PWM then we need to use the device tree overlay to choose the pin as a PWM. For example, we want to use pin P9_22 as PWM, which is configured as SPI. 
 If you try to run example fade_dt.py:
 
 ```
 python fade_dt.py
 ```
 
You will get a runtime error stating that you need to load the PWM channel first. In order to load the channel, we do the following:
```
cd /lib/firmware
ls bone_pwm*
```
Below are complied dtbo device tree overlays for the avaiable pwm pins:
```
bone_pwm_P8_13-00A0.dtbo  bone_pwm_P8_46-00A0.dtbo  bone_pwm_P9_28-00A0.dtbo
bone_pwm_P8_19-00A0.dtbo  bone_pwm_P9_14-00A0.dtbo  bone_pwm_P9_29-00A0.dtbo
bone_pwm_P8_34-00A0.dtbo  bone_pwm_P9_16-00A0.dtbo  bone_pwm_P9_31-00A0.dtbo
bone_pwm_P8_36-00A0.dtbo  bone_pwm_P9_21-00A0.dtbo  bone_pwm_P9_42-00A0.dtbo
bone_pwm_P8_45-00A0.dtbo  bone_pwm_P9_22-00A0.dtbo

```

Now, to load the file associated with P9_22:

```
sudo sh -c "echo bone_pwm_P9_29 > $SLOTS"
sudo sh -c "echo am33xx_pwm > $SLOTS"
```

Now if you do cat $SLOTS, the last slots shows the pwm pin:
```
 0: 54:PF--- 
 1: 55:PF--- 
 2: 56:PF--- 
 3: 57:PF--- 
 4: ff:P-O-L Bone-LT-eMMC-2G,00A0,Texas Instrument,BB-BONE-EMMC-2G
 5: ff:P-O-L Bone-Black-HDMI,00A0,Texas Instrument,BB-BONELT-HDMI
 7: ff:P-O-L Override Board Name,00A0,Override Manuf,am33xx_pwm
18: ff:P-O-L Override Board Name,00A0,Override Manuf,bone_pwm_P9_22
```
Many of the PWm pins are being used by the HDMI cape. if you need to use these pins, the HDMi cape has to be removed. This can be done by changing the uEnv file in /boot. 

## ADC
Beaglebone has 7 analog input channels (12 bit, 125 ns Sample time). These input pins are 1.8V tolerant. Therefore, we need to make sure than the voltage of the senor is propoerly condiioned so that it is in the range of 0-1.8V. 

Example folder contains a script adc.py which shows how to use the ADC pin on beagle bone black. ADC is avialble on the following pins:

```
P9_39   AIN0
P9_40   AIN1
P9_37   AIN2
P9_38   AIN3
P9_33   AIN4
P9_36   AIN5
P9_35   AIN6
```

To use an ADC, we need to do the following:

```
1. Setup
ADC.setup()

2a. Read - returns value in range 0 - 1.0
ADC.read(ADC Number)

or 

2b. Read raw values
ADC.read_raw(ADC number)
```






