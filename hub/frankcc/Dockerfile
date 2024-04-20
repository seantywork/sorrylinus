FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /home

COPY . .

RUN	apt-get update

RUN apt-get -y install git

RUN apt-get -y install build-essential make

RUN	make dep-package

RUN	make release

CMD ["./drun.sh"]