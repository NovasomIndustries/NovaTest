#!/bin/sh

#P-Line_P7D
#collaudatore: mp


ERROR=""
NCORE=`cat /proc/cpuinfo | grep processor | wc -l`
CPU=`dmesg | grep processors | grep $NCORE`
if [ -n "$CPU" ]
then
echo CPU OK >> report.txt
else

unset CPU
 echo CPU KO >> report.txt
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
WiFi=`dmesg | grep wifi | grep Done`
if [ -n "$WiFi" ]
then
echo WIFI OK >> report.txt
else
unset WiFi
echo WIFI KO >> report.txt
fi
[ "$WiFi" == "" ] && ERROR="WiFi Error"
if [ "$ERROR" == "" ]; then
RES="OK"
else
RES="KO"
fi
rm b*
get_exec b.php?argument1=$RES
