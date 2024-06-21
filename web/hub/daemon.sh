#!/bin/bash

while true
do

    CHECK=$(pgrep -f /home/hubc.out | grep "" -c)
    if [ $CHECK -eq 0 ]
    then
        /home/hubc.out
    fi 
    sleep 5

done