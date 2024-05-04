#!/bin/bash

while true
do

    CHECK=$(pgrep /home/frank | grep "" -c)
    if [ $CHECK -eq 0 ]
    then
        /home/frank
    fi 
    sleep 3

done