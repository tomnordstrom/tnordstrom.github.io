#!/bin/bash
if [[ $(cat /proc/device-tree/model) == "Raspberry Pi 3"* ]];
then
  sudo ./lmaxgui_qt5_rpi3 640 480 800 480 900 -style windows
else
  sudo ./lmaxgui_qt5_rpi4 640 480 800 480 900 -platform linuxfb
fi
