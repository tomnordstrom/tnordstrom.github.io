#!/bin/bash
./killgui.sh
sleep 1
./startgui.sh

#/home/pi/machine/shmUtil -set killgui 1
#if [[ $(cat /proc/device-tree/model) == "Raspberry Pi 3"* ]];
#then
#  sudo killall lmaxgui_qt5_rpi3
#fi
#sleep 1
#/home/pi/machine/shmUtil -set killgui 0
