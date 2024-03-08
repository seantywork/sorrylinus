#!/bin/bash





kubectl -n frank delete -f ./k8s/ingress.yaml

kubectl -n frank delete -f ./k8s/ingress-front.yaml

kubectl delete -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

mv /etc/nginx/conf.d/fd.conf.org /etc/nginx/conf.d/fd.conf 


systemctl restart nginx

certbot --nginx --rsa-key-size 4096 --no-redirect 

mv /etc/nginx/conf.d/fd.conf /etc/nginx/conf.d/fd.conf.org

systemctl restart nginx

kubectl -n frank delete secret tls frank-tls-secret

kubectl -n frank create secret tls frank-tls-secret --key /etc/letsencrypt/live/feebdaed.xyz/privkey.pem --cert /etc/letsencrypt/live/feebdaed.xyz/fullchain.pem 

kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml


RETRY_LIMIT=300
RETRY_COUNT=0

while [ $RETRY_COUNT -lt $RETRY_LIMIT ]
do
    if kubectl get deployment -n ingress-nginx | grep -q 'ingress-nginx-controller   1/1'
    then
        echo "ingress ready"
        break;
    else
        echo "waiting for ingress to be ready"
    fi

    RETRY_COUNT=$(( $RETRY_COUNT + 1 ))

    sleep 1

done

kubectl -n frank apply -f ./k8s/ingress.yaml

kubectl -n frank apply -f ./k8s/ingress-front.yaml
