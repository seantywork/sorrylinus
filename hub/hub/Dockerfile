FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /home

COPY . .

RUN	apt-get update

RUN apt-get -y install git

RUN apt-get -y install build-essential

RUN	apt-get -y install sqlite3 libsqlite3-dev

RUN	apt-get -y install libmysqlcppconn-dev
	
RUN	apt-get -y install libboost-all-dev

RUN apt-get -y install libssl-dev

RUN mkdir -p ccwslib

RUN git -C ./ccwslib init

RUN git -C ./ccwslib pull https://github.com/seantywork/websocketpp.git master 

RUN mkdir -p /usr/local/include/websocketpp

RUN /bin/cp -rf ./ccwslib/websocketpp/* /usr/local/include/websocketpp

RUN rm -rf ./ccwslib

RUN	g++ -pthread -o hub server.cc -lssl -lcrypto -lmysqlcppconn

CMD ["./drun.sh"]