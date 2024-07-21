#!/bin/bash


sudo mkdir -p /data/sorrylinus-again


sudo mkdir -p /data/sorrylinus-again/data/admin

sudo echo '{}' | sudo tee /data/sorrylinus-again/data/admin/seantywork@gmail.com.json

sudo mkdir -p /data/sorrylinus-again/data/media

sudo mkdir -p /data/sorrylinus-again/data/media/article

sudo mkdir -p /data/sorrylinus-again/data/media/image

sudo mkdir -p /data/sorrylinus-again/data/media/video

sudo mkdir -p /data/sorrylinus-again/data/session


sudo mkdir -p /data/sorrylinus-again/data/user



sudo chmod -R 777 /data/sorrylinus-again

sudo kubectl apply -n frank -f ../k8s/app-volume.yaml