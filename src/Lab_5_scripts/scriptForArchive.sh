#!/bin/bash


isWorking=1

echo "start to archive"
name_of_catalogue=$1
way_to_archive=$2
period=$3

while [ $isWorking ]
do
	cd ~
	zip -r archive.zip $1
	mv archive.zip $2
	sleep $3
done
