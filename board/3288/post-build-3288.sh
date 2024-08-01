#!/bin/bash

TARGET=$BUILD_DIR/../target

rm -rf $TARGET/etc/init.d/S01syslogd
rm -rf $TARGET/etc/init.d/S02klogd
rm -rf $TARGET/etc/init.d/S02sysctl
rm -rf $TARGET/etc/init.d/S20urandom
rm -rf $TARGET/etc/init.d/S40network
if [ -f $TARGET/etc/init.d/S40bluetooth ]; then
	mv $TARGET/etc/init.d/S40bluetooth $TARGET/etc/init.d/mS40bluetooth
fi
if [ -f $TARGET/etc/init.d/S35iptables ]; then
	mv $TARGET/etc/init.d/S35iptables $TARGET/etc/init.d/mS35iptables
fi
