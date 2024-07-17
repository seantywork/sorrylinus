
sudo vim /etc/default/coturn

TURNSERVER_ENABLED=1

sudo systemctl start coturn


sudo vim /etc/turnserver.conf

listening-ip=0.0.0.0

listening-port=3478

min-port=3479
max-port=3579


user=MYUSER:MYPASS


sudo systemctl restart coturn