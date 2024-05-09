FROM debian:12

ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /home

COPY . .

RUN	apt-get update

RUN apt-get -y install git

RUN apt-get -y install build-essential make procps

RUN make dep-package

RUN make dep-source

RUN	make release

#CMD ["tail", "-f","/dev/null"]

CMD ["./frankc.out"]