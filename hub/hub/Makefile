

GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c


DEP_PACKAGES := libmysqlcppconn-dev libboost-all-dev libssl-dev


INCLUDES := -I./include

LIBS := -lpthread -lssl -lcrypto -lmysqlcppconn  



OBJS := core_server.o
OBJS += core_sock_resource.o
OBJS += core_path_handle.o
OBJS += core_peer_verification.o
OBJS += core_db_query.o


all: 

	@echo "frank hub: dev, release"

dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dep-source:

	mkdir -p ccwslib

	mkdir -p ./include/websocketpp

	git -C ./ccwslib init

	git -C ./ccwslib pull https://github.com/seantywork/websocketpp.git master 

	/bin/cp -rf ./ccwslib/websocketpp/* ./include/websocketpp

	sudo rm -r ./ccwslib

dev: $(OBJS)

	g++ $(GCC_DEV_FLAGS) $(INCLUDES) -o hub main.cc $(OBJS) $(LIBS) 


release: $(OBJS)

	g++ $(GCC_REL_FLAGS) $(INCLUDES) -o hub main.cc $(OBJS) $(LIBS) 



core_server.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o core_server.o core/server.cc 


core_sock_resource.o:


	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o core_sock_resource.o core/sock_resource.cc 

core_path_handle.o:


	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o core_path_handle.o core/path_handle.cc 

core_peer_verification.o:


	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o core_peer_verification.o core/peer_verification.cc 


core_db_query.o:


	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o core_db_query.o core/db_query.cc 



clean :

	rm -rf *.o *.out hub


