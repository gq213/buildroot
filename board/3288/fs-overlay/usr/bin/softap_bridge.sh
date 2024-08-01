#!/bin/sh

ifconfig eth0 0.0.0.0 up
ifconfig wlan0 0.0.0.0 up
hostapd -B /etc/ap_5g.conf -dd
brctl addbr br0
brctl addif br0 eth0
brctl addif br0 wlan0
brctl show br0
ifconfig br0 up
udhcpc -b -i br0 -x hostname:br0 -R -s /etc/simple.script &
