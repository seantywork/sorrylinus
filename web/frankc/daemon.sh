#!/bin/bash

while true
do

    CHECK=$(pgrep -f /home/frankc.out | grep "" -c)
    if [ $CHECK -eq 0 ]
    then
        /home/frankc.out
    fi 
    sleep 5

done