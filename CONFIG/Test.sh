#!/bin/sh

echo "Informazioni sul sistema:"
echo -e "hardware:\t$(uname -m)"

BOARD=`dmesg | awk '/Found board/ {print $3}'`
echo -e "Board:\t$BOARD"
echo -e "OS:\t$(uname -o)"

echo "Informazioni sull'utente:"
echo -e "logname:\t$LOGNAME"
echo -e "homedir:\t$HOME"
echo -e "shell:\t$SHELL"
echo -e "path:\t$PATH\n"

echo "Checking console..."
CONSOLE=`cat /proc/consoles | awk '{print $1}'`
if  dmesg | grep console | grep $CONSOLE > /dev/null 
then
	echo "CONSOLE OK"
else 
	echo "CONSOLE KO"
	unset CONSOLE
fi

echo "Checking CPU..."
CPU=`cat /proc/cpuinfo | grep processor | wc -l`
if [ "$CPU" = `dmesg | awk '/processors/ {print$4}'` ]
then
	echo "CPU OK, $CPU core(s)."
else
	echo "CPU KO, incoherent number of core"
	unset CPU 
fi

echo "Checking SDcard reader..."

SDCard=`cat /sys/bus/mmc/devices/mmc0\:0007/name`
if [ "$SDCard" = `dmesg | awk '/SD04G/ {print $3}'` ]
then
	echo "SDcard OK."
else
	echo "SDcard KO."
	unset SDCard
fi


echo "Checking eMMC..."
time sh -c "dd if=dev/zero of=/dev/mmcblk1 bs=4K count=1 && sync" 
#EMMC=`cat /sys/bus/mmc/devices/mmc2\:0001/name`
#if [ $EMMC = `dmesg | grep 3.57 | awk '/MMC04G/ {print $3}'` ]
#then
#	echo "eMMC OK."
#else
#	echo "eMMC KO."
#	unset EMMC
#fi

echo "Checking RAM..."
RAM=`cat /proc/meminfo | grep MemTotal | awk '{ print $2$3 }'`
if [ -n "$RAM" ]
then
	echo "RAM disponibile: $RAM"
else
	echo "Problema sulla RAM"
	unset RAM
fi

echo "Checking RTC..."
RTC=`dmesg | grep rtc | grep setting | awk ' { print $1,$7,$8,$9 }'`
if [ -n "$RTC" ]
then
	echo " $RTC"
else
	echo "Problema con RTC"
	unset RTC
fi

echo "Checking Audio"
AUDIO=`dmesg | grep asoc | wc -l`
if [ -n "$AUDIO" ]
then
	echo "Audio found!"
	echo "eseguire test audio? [Y/N]"
	read ANSWER
	if [ "$ANSWER" = "Y" ]
	then
		echo "Plug in device"
		echo "premere un tasto per continuare..."
		read
		gplay Processed_White_Noise.mp3
	fi
else 
	echo "Audio Not Found!"
	unset AUDIO
fi 

echo "Checking USB..."
USB=`ls /sys/bus/usb/devices | grep usb | wc -l`
if [ $USB -eq `dmesg | grep "1 port" | wc -l` ]
then
	echo "Trovate $USB porte usb"
else
	echo " Problema su porta usb..."
	unset USB
fi

echo "Checking Ethernet..."
ETH=`dmesg | grep eth0`
if [ -n "$ETH" ]
then
	echo "Ethernet found"
	
else
	echo "problema su eth0"
	unset ETH
fi

echo "Checking Wifi..."
WiFi=`dmesg | grep wifi | grep Done`
if [ -n "$WiFi" ]
then
	echo "WiFi correctly initializing"
else
	echo "Wifi non correttamente inizializzato"
	unset WiFi
fi

echo "Eseguire test connettività? [Y/N]"
	read ANSWER
	if [ "$ANSWER" = "Y" ]
	then
	      echo "Establish a connection..."
	      echo "premere un tasto per continuare..."
	      read
	      while [ ! -f /tmp/my_ip ]; do
		    sleep 1
		    echo "waiting ip..."
	      done
	      echo "ip: `cat /tmp/my_ip`"
	fi

echo "Checking HDMI..."
HDMI=``
if [ -n "$HDMI" ]
then
	echo ""
else
	echo ""
	unset HDMI
fi

echo "Checking LVDS..."
LVDS=``
if [ -n "$LVDS" ]
then
	echo ""
else
	echo ""
	unset LVDS
fi

echo "Checking Touch..."
TOUCH=``
if [ -n "$TOUCH" ]
then
	echo ""
else
	echo ""
	unset TOUCH
fi


ERROR=""
[ "$CONSOLE" == "" ] && ERROR="Console Error"
[ "$CPU" == "" ] && ERROR="CPU Error"
[ "$SDCard" == "" ] && ERROR="SDcard Error"
[ "$EMMC" == "" ] && ERROR="eMMC Error"
[ "$RAM" == "" ] && ERROR="RAM Error"
[ "$RTC" == "" ] && ERROR="RTC Error"
[ "$AUDIO" == "" ] && ERROR="Audio Error"
[ "$USB" == "" ] && ERROR="USB Error"
[ "$ETH" == "" ] && ERROR="Ethernet Error"
[ "$WiFi" == "" ] && ERROR="WiFi Error"
[ "$HDMI" == "" ] && ERROR="HDMI Error"
[ "$LVDS" == "" ] && ERROR="LVDS Error"
[ "$TOUCH" == "" ] && ERROR="Touch Error"



if [ "$ERROR" == "" ]; then
	echo "Test Done, board OK"
else
	echo "BOARD FAILED: $ERROR"
fi
