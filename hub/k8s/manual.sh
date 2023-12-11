#!/bin/bash


kubectl create namespace frank

kubectl create secret generic docker-secret --from-file=.dockerconfigjson=/root/.docker/config.json --type=kubernetes.io/dockerconfigjson -n frank

kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

kubectl -n frank apply -f ./app.yaml

kubectl -n frank apply -f ./ingress.yaml

kubectl -n frank apply -f ./ingress-front.yaml