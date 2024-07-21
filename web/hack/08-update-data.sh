#!/bin/bash


sudo mkdir -p /data/sorrylinus-again

sudo chmod -R 777 /data/sorrylinus-again

sudo kubectl apply -n frank -f ../k8s/app-volume.yaml