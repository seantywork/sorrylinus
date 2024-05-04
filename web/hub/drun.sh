#!/bin/bash

while true
do

    CHECK=$(pgrep /home/hub | grep "" -c)
    if [ $CHECK -eq 0 ]
    then
        /home/hub
    fi 
    sleep 3

done