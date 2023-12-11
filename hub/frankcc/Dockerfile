FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /home

COPY . .

RUN	apt-get update

RUN apt-get -y install git

RUN apt-get -y install build-essential

RUN	apt-get -y install libmysqlcppconn-dev

RUN	apt-get install -y libssl-dev 
	
RUN	apt-get install -y libcurl4-openssl-dev

RUN mkdir -p cchttplib

RUN mkdir -p json

RUN git -C ./cchttplib init

RUN git -C ./json init

RUN git -C ./cchttplib pull https://github.com/seantywork/cpp-httplib.git master 

RUN git -C ./json pull https://github.com/seantywork/json.git develop

RUN mkdir -p /usr/local/include/httplib

RUN mkdir -p /usr/local/include/jsonlib

RUN /bin/cp -rf ./cchttplib/httplib.h /usr/local/include/httplib/httplib.h

RUN /bin/cp -rf ./json/single_include/nlohmann/* /usr/local/include/jsonlib

RUN rm -rf ./cchttplib

RUN rm -rf ./json

RUN	g++ -pthread -o frank frank.cc -lmysqlcppconn -lcurl

CMD ["./drun.sh"]