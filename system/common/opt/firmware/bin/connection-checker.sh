#!/bin/sh /etc/rc.common

interval_hours=12
hosts="mios.com google.com 1.1.1.1 8.8.8.8"
ping_interval_minutes=60

isConnected(){
    for host in $hosts; 
      do
        if [ ! -z "$1" ]; then
          echo $1
          if ping -I $1 -q -w 1 -c 1 $host > /dev/null; then
              return 1
          fi
        else
          if ping -q -w 1 -c 1 $host > /dev/null; then
              return 1
          fi
        fi 
       done
      return 0
}

ping_interval=$(($ping_interval_minutes*60))

while :
do
    isConnected
    if [ $? -eq 1 ];
    then
      count=0
    else 
      count=$(($count + 1))
    fi

    if [ $count -gt $interval_hours ]; then
      reboot
    fi
sleep $ping_interval
done
