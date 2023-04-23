#!/bin/sh
ulimit -c unlimited
sudo echo "/data/core-%e-%p-%t" > /proc/sys/kernel/core_pattern
./factory -g 
