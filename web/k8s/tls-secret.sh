#!/bin/bash


kubectl -n frank delete secret tls frank-tls-secret --ignore-not-found

kubectl -n frank create secret tls frank-tls-secret --key /etc/letsencrypt/live/feebdaed.xyz/privkey.pem --cert /etc/letsencrypt/live/feebdaed.xyz/fullchain.pem
