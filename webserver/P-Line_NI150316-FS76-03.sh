#!/bin/sh

#P-Line_P6B


echo CONSOLE OK >> report.txt
echo EEPROM OK >> report.txt
echo SDCARD OK >> report.txt

fb-test


ERROR=""
NCORE=`cat /proc/cpuinfo | grep processor | wc -l`
CPU=`dmesg | grep processors | grep $NCORE`
if [ "$NCORE" -eq 2 ]
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
if [ "$ERROR" == "" ]; then
RES="OK"
else
RES="KO"
fi
rm b*
get_exec b.php?argument1=$RES
