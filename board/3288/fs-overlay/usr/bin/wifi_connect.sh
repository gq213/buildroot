#!/bin/sh

wpa_supplicant -B -Dnl80211 -iwlan0 -c /etc/sta_5g.conf -dd
