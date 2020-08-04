#!/bin/sh

#M-Line_M7FT

while [ ! -f /tmp/my_ip ]; do
	sleep 1
	echo "Waiting for DHCP"
done
echo Starting Tests
sleep 1

echo "System Information:">> report.txt
echo -e " hardware:\t$(uname -m)" >> report.txt

echo -e "Board : N" >> report.txt
echo -e "OS:\t$(uname -o)" >> report.txt

echo SDCARD OK >> report.txt
echo CONSOLE OK >> report.txt

fb-test

ERROR=""

echo "Checking CPU..." >> report.txt
CPU=`cat /proc/cpuinfo | grep processor | wc -l`
if [ "$CPU" = `dmesg | awk '/processors/ {print $4}'` ]
then
	echo "CPU OK, $CPU core(s)." >> report.txt
else
	echo "CPU KO, incoherent number of core" >> report.txt
	unset CPU
fi
[ "$CPU" == "" ] && ERROR="CPU Error"

ETH=`dmesg | grep eth0`
if [ -n "$ETH" ]
then
echo Ethernet OK >> report.txt
else
unset ETH
echo Ethernet KO >> report.txt
fi
[ "$ETH" == "" ] && ERROR="Ethernet Error"

RAM=`cat /proc/meminfo | grep MemTotal `
if [ -n "$RAM" ]
then
echo RAM OK >> report.txt
else
unset RAM
echo RAM KO >> report.txt
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



if [ "$ERROR" == "" ]; then
RES="OK"
else
RES="KO"
fi
rm b*
get_exec b.php?argument1=$RES