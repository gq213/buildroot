#!/bin/bash

TARGET=$BUILD_DIR/../target

if [ -f $TARGET/etc/init.d/S01seedrng ]; then
	mv $TARGET/etc/init.d/S01seedrng $TARGET/etc/init.d/mS01seedrng
fi
if [ -f $TARGET/etc/init.d/S01syslogd ]; then
	mv $TARGET/etc/init.d/S01syslogd $TARGET/etc/init.d/mS01syslogd
fi
if [ -f $TARGET/etc/init.d/S02klogd ]; then
	mv $TARGET/etc/init.d/S02klogd $TARGET/etc/init.d/mS02klogd
fi
if [ -f $TARGET/etc/init.d/S02sysctl ]; then
	mv $TARGET/etc/init.d/S02sysctl $TARGET/etc/init.d/mS02sysctl
fi
if [ -f $TARGET/etc/init.d/S40network ]; then
	mv $TARGET/etc/init.d/S40network $TARGET/etc/init.d/mS40network
fi
