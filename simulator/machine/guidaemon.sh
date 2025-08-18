#!/bin/bash
#ls -alp
while true; do
LABEL=$(/home/pi/machine/shmUtil -read killgui)
echo $LABEL

if [ "$LABEL" = "0" ]; then
  #echo "x has the value 0"
  sleep 1
  /home/pi/machine/startlmaxgui.sh
  sleep 1
fi
if [ "$LABEL" = "1" ]; then
  #echo "x has the value 1"
  sleep 1
fi
done
