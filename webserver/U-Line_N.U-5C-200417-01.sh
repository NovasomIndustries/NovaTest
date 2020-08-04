
#!/bin/sh

#U-Line_U5C

while [ ! -f /tmp/my_ip ]; do
	sleep 1
	echo "Waiting for DHCP"
done
echo Starting Tests
sleep 1


echo "System Information:">> report.txt
echo -e " hardware:\t$(uname -m)" >> report.txt
echo -e "Board: NOVAsom U5C" >> report.txt
echo -e "OS:\t$(uname -o)" >> report.txt

echo "Checking Console..." >> report.txt
echo CONSOLE OK >> report.txt
echo "Checking SdCard..." >> report.txt
echo SDCARD OK >> report.txt

echo "Checking CPU..." >> report.txt
ERROR=""
NCORE=`cat /proc/cpuinfo | grep processor | wc -l`
CPU=`dmesg | grep processors | grep $NCORE`
if [ "$NCORE" -eq 1 ]
then
echo "CPU OK, $CPU core(s)." >> report.txt
cat /proc/cpuinfo >> report.txt
else
echo "CPU KO, incoherent number of core" >> report.txt
unset CPU
fi
[ "$CPU" == "" ] && ERROR="CPU Error"



ETH=`dmesg | grep eth0`
echo "Checking Ethernet..." >> report.txt
if [ -n "$ETH" ]
then
echo "Ethernet found" >> report.txt
else
echo "problema su eth0" >> report.txt
unset ETH
fi
[ "$ETH" == "" ] && ERROR="Ethernet Error"


echo "Checking RAM" >> report.txt
RAM=`cat /proc/meminfo | grep MemTotal `
if [ -n "$RAM" ]
then
	echo "RAM OK. Disponibile : $RAM" >> report.txt
	cat /proc/meminfo >> report.txt
else
	echo "RAM KO" >> report.txt
	unset RAM
fi
[ "$RAM" == "" ] && ERROR="RAM Error"

RTC=`dmesg | grep rtc | grep setting `
if [ -n "$RTC" ]
then
echo RTC OK >> report.txt
else
unset RTC
 echo RTC KO >> report.txt
fi
[ "$RTC" == "" ] && ERROR="RTC Error"

echo "Checking USB..." >> report.txt
USB=`ls /sys/bus/usb/devices | grep usb | wc -l`
#if [ $USB -eq `dmesg | grep "1 port" | wc -l` ]
if [ $USB -ge "1" ]
then
	#echo "Trovate $USB porte usb" 
	echo "USB Ok" >> report.txt
else
	echo " Problema su porta usb..." >> report.txt
	unset USB
fi
[ "$USB" == "" ] && ERROR="USB Error"

echo "Checking Video..." >> report.txt
fb-test

if [ "$ERROR" == "" ]; then
RES="OK"
else
RES="KO"
fi


rm b*
get_exec b.php?argument1=$RES