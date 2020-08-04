#!/bin/sh
while [ ! -f /tmp/my_ip ]; do
	sleep 1
	echo "Waiting for DHCP"
done
echo Starting Tests
sleep 1

echo "System Information:">> report.txt
echo -e " hardware:\t$(uname -m)" >> report.txt


BOARD=`dmesg | awk '/board/ {print $3}'`
echo -e "Board : NI240613-FS05 GDS Module" >> report.txt
echo -e "OS:\t$(uname -o)" >> report.txt

echo "Checking CPU..." >> report.txt
CPU=`cat /proc/cpuinfo | grep processor | wc -l`
if [ "$CPU" = `dmesg | awk '/processors/ {print $4}'` ]
then
	echo "CPU OK, $CPU core(s)." >> report.txt
else
	echo "CPU KO, incoherent number of core" >> report.txt
	unset CPU
fi

#echo "Checking eMMC..."

echo "Checking RAM" >> report.txt
RAM=`cat /proc/meminfo | grep MemTotal | awk '{ print $2$3}'`
if [ -n "RAM" ]
then
	echo "RAM OK. Disponibile : $RAM" >> report.txt
else
	echo "RAM KO" >> report.txt
	unset RAM
fi

echo "Checking RTC..." >> report.txt
RTC=`dmesg | grep rtc | grep setting | awk ' { print $1,$7,$8,$9 }'`
if [ -n "$RTC" ]
then
	echo " $RTC" >> report.txt
else
	echo "Problema con RTC" >> report.txt
	unset RTC
fi

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

echo "Checking Ethernet..." >> report.txt
ETH=`dmesg | grep eth0`
if [ -n "$ETH" ]
then
	echo "Ethernet found" >> report.txt
	
else
	echo "problema su eth0" >> report.txt
	unset ETH
fi

echo "Checking eMMC..." >> report.txt
echo "Formatting data..." >> report.txt
/tmp/application_storage/AutoRun_1.sh >> /dev/null
sync
echo "Complete." >> report.txt

echo "Checking LVDS..." >> report.txt

sleep 1
fb-test


#echo "Can you see video?[Y|N]"
#read LVDS 
#if [ "$LVDS" = "Y" ]
#then
#	echo "LVDS OK"
#else
#	echo "LVDS KO"
#	unset LVDS
#fi

ERROR=""
[ "$CPU" == "" ] && ERROR="CPU Error"
#[ "$EMMC" == "" ] && ERROR="eMMC Error"
[ "$RAM" == "" ] && ERROR="RAM Error"
[ "$RTC" == "" ] && ERROR="RTC Error"
[ "$USB" == "" ] && ERROR="USB Error"
[ "$ETH" == "" ] && ERROR="Ethernet Error"
#[ "$LVDS" == "" ] && ERROR="LVDS Error"

if [ "$ERROR" == "" ]; then
	RES="OK"
	echo ""
	echo "		Test Done, board OK"
else
	RES="KO"
	echo ""
	echo ""
	echo ""
	echo ""
	echo "################################################"
	echo "   	    BOARD FAILED: $ERROR"
	echo "################################################"
fi

rm b*
get_exec b.php?argument1=$RES



