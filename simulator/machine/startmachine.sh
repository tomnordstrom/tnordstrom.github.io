#!/bin/bash
while [! -d "/home/pi/machine/shmBuild" ]; do
	sleep 1
done
sleep 1
sudo ip -4 addr add 192.168.1.50/24 dev eth0  label eth0:1
mpg123 /home/pi/machine/sound/Startup.mp3 &
cd /home/pi/machine/shmBuild
./startUp.sh
cd ..
./guidaemon.sh &
./ebmComm > /dev/null &
./simulator > /dev/null &
#sudo ./modbusser&
