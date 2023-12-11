#!/bin/bash


kubectl create namespace frank

kubectl create secret generic docker-secret --from-file=.dockerconfigjson=/root/.docker/config.json --type=kubernetes.io/dockerconfigjson -n frank

kubectl -n frank create secret tls frank-tls-secret --key /etc/letsencrypt/live/feebdaed.xyz/privkey.pem --cert /etc/letsencrypt/live/feebdaed.xyz/fullchain.pem 

kubectl -n frank apply -f ./app.yaml

kubectl -n frank apply -f ./ingress.yaml

kubectl -n frank apply -f ./ingress-front.yaml