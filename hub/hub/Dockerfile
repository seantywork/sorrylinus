FROM ubuntu:20.04

WORKDIR /home

COPY . .

RUN	sudo apt-get update

RUN	sudo apt-get -y install sqlite3 libsqlite3-dev

RUN	sudo apt-get -y install libmysqlcppconn-dev
	
RUN	sudo apt-get install -y libboost-all-dev

RUN	g++ -pthread -o hub server.cc -lssl -lcrypto -lmysqlcppconn

CMD ["./hub"]