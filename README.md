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

## GPIO
