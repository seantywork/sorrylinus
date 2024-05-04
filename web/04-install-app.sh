#!/bin/bash


kubectl create namespace frank

kubectl create secret generic docker-secret --from-file=.dockerconfigjson=/root/.docker/config.json --type=kubernetes.io/dockerconfigjson -n frank

kubectl -n frank apply -f ./k8s/app.yaml

kubectl -n kube-system rollout restart deployment/coredns