#!/bin/bash

COUNTER=0

echo "its the first script"

while [ $COUNTER -lt 1000 ]
do
	echo "in first script"
	sleep 10
	COUNTER=$(($COUNTER+1))
done
