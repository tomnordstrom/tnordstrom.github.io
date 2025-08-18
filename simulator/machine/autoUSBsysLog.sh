mediaPath=/home/pi/fakeMedia/
shmUtilPath=/home/pi/tobbe/svn/shmUtil/trunk/shmUtil
usbPath=noDevice
usbPath=$(ls -1t /media/pi/ | head -1)
echo $usbPath



while [ 1 ]
do

   if [ -z "$(ls -1t /media/pi/ | head -1)" ]; then
     echo nej
   else
     echo jo 
     usbPath=$(ls -1t /media/pi/ | head -1)
     sysLogfile=/sysLog
     mediaPath=/media/pi/$usbPath
     echo $mediaPath
     usbPath=$mediaPath$sysLogfile
     echo $usbPath
     ###echo "knulla" > $usbPath

$shmUtilPath -all -nc > $usbPath
echo ""  >> $usbPath
echo "************************* FIFOs *************************"  >> $usbPath
echo "" >> $usbPath
$shmUtilPath -fifo >> $usbPath
echo "" >> $usbPath
echo "******************** shmUtil Version ********************" >> $usbPath
echo "" >> $usbPath
$shmUtilPath -version >> $usbPath
echo "" >> $usbPath
echo "********************* shared memory *********************" >> $usbPath
echo "" >> $usbPath
$shmUtilPath -shm >> $usbPath
echo "" >> $usbPath
echo "************************** ps **************************" >> $usbPath
echo "" >> $usbPath
ps -ef >> $usbPath
echo "" >> $usbPath
echo "************************** df **************************" >> $usbPath
echo "" >> $usbPath
df -h >> $usbPath
echo "" >> $usbPath
echo "************************* free *************************" >> $usbPath
echo "" >> $usbPath
free -m >> $usbPath
echo "" >> $usbPath
echo "********************* temperature *********************" >> $usbPath
echo "" >> $usbPath
vcgencmd measure_temp >> $usbPath
echo "" >> $usbPath
echo "*********************** ifconfig ***********************" >> $usbPath
echo "" >> $usbPath
ifconfig >> $usbPath
echo "" >> $usbPath
echo "*********************** date ***********************" >> $usbPath
echo "" >> $usbPath
date >> $usbPath

     mediaPath=usbInserted
     while [ $mediaPath = "usbInserted" ]
     do
       sleep 5
        if [ -z "$(ls -1t /media/pi/ | head -1)" ]; then
          mediaPath=usbRemoved
          echo "usb removed"
        else
          mediaPath=usbInserted
          echo "usb inserted" 
        fi 
     done
     echo "usb removed 2"
   fi

   sleep 1
done
