#!/bin/bash


source netutils.sh
lineeth0=$(grep -n 'interface eth0' /etc/dhcpcd.conf | cut -d: -f1)
lineeth1=$(grep -n 'interface eth1' /etc/dhcpcd.conf | cut -d: -f1)
echo $lineeth0
echo $lineeth1
#lineeth0=$(($lineeth0 + 1))

if [ "$1" != "eth0" ]; then
	if [ "$1" != "eth1" ]; then
		echo "usage: chIp eth[x] ip netmask gw\n"
		echo "example: chIp eth0 192.168.0.50 255.255.255.0 192.168.0.1"
		exit
	fi
fi
if [ "$2" = "" ]; then
	echo "Too few parameters\n"
	echo "usage: chIp eth[x] ip netmask gw\n"
	echo "example: chIp eth0 192.168.0.50 255.255.255.0 192.168.0.1"
		exit
fi
if [ "$3" = "" ]; then
	echo "Too few parameters\n"
	echo "usage: chIp eth[x] ip netmask gw\n"
	echo "example: chIp eth0 192.168.0.50 255.255.255.0 192.168.0.1"
		exit
fi
if [ "$4" = "" ]; then
	echo "Too few parameters\n"
	echo "usage: chIp eth[x] ip netmask gw\n"
	echo "example: chIp eth0 192.168.0.50 255.255.255.0 192.168.0.1"
		exit
fi

#currdns=$(cat /etc/dhcpcd.conf | grep -e '^static domain_name_servers=' | cut -d= -f2)

if [ "$1" = "eth0" ]; then

	currip=$(cat /etc/dhcpcd.conf | grep -e '^static ip_address=' | cut -d= -f2 |  awk 'NR>=1 && NR<=1')
	currnm=$(cat /etc/dhcpcd.conf | grep -e '^static netmask=' | cut -d= -f2 |  awk 'NR>=1 && NR<=1')
	currgw=$(cat /etc/dhcpcd.conf | grep -e '^static routers=' | cut -d= -f2 |  awk 'NR>=1 && NR<=1')
	lineethx=$lineeth0 
fi
if [ "$1" = "eth1" ]; then
	currip=$(cat /etc/dhcpcd.conf | grep -e '^static ip_address=' | cut -d= -f2 |  awk 'NR>=2 && NR<=2')
	currnm=$(cat /etc/dhcpcd.conf | grep -e '^static netmask=' | cut -d= -f2 |  awk 'NR>=2 && NR<=2')
	currgw=$(cat /etc/dhcpcd.conf | grep -e '^static routers=' | cut -d= -f2 |  awk 'NR>=2 && NR<=2')
	lineethx=$lineeth1
fi

echo "current IP$1 is $currip"
echo "current nm$1 is $currnm"
echo "current GW$1 is $currgw"

if [ "$currip" != "$2" ]; then
	echo "not same ip"
	changes="true"
fi
if [ "$currnm" != "$3" ]; then
	echo "not same nm"
	changes="true"
fi
if [ "$currgw" != "$4" ]; then
	echo "not same gw"
	changes="true"
fi

echo "$currip $2 $changes"

if [ "$changes" != "true" ]; then
echo "no changes do nothing"
exit
fi

if [[ $(cat /proc/device-tree/model) != "Raspberry Pi 3"* ]];
then
	sudo mount -o remount -orw /
fi

echo $lineethx
#replacing ethX ip netmask and gw
mask=$(netmask2cidr $3)
sed -i -e $lineethx','$(($lineethx + 4))'s'"@static ip_address=.*\b@static ip_address=$2/$mask@g" /etc/dhcpcd.conf
sed -i -e $lineethx','$(($lineethx + 4))'s'"@static netmask=.*\b@static netmask=$3@g" /etc/dhcpcd.conf
sed -i -e $lineethx','$(($lineethx + 4))'s'"@static routers=.*\b@static routers=$4@g" /etc/dhcpcd.conf

echo "$1"

if [[ $(cat /proc/device-tree/model) != "Raspberry Pi 3"* ]];
then
	sudo mount -o remount -oro /
fi
sudo ifconfig "$1" down 
sudo ifconfig "$1" up

