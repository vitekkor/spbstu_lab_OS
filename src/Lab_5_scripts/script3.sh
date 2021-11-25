#!/bin/bash

COUNTER=0

while [ $COUNTER -lt 1000 ]
do
	read info
	echo "input ----> $info"
	sleep 10
	COUNTER=$(($COUNTER+1))
done
