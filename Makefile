
GCC_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

GCC_LIB_FLAGS := -Wall -shared

GCC_LIB_OBJ_FLAGS := -Wall -c -fpic 

DEP_PACKAGES := sqlite3 libsqlite3-dev libssl-dev libboost-all-dev

INCLUDES := -I./include

LINKS := -L./lib

LOADS := -Wl,-rpath=./lib

LIBS := -lsolimod -lsqlite3 -lssl -lcrypto



SOLIMOD_OBJS := solimod.o

APP_OBJS := client.o 
APP_OBJS += client_mod.o
APP_OBJS += models.o



all:

	@echo "sorrylinus"


dep-package:


	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dep-source:

	mkdir -p ccwslib

	mkdir -p json

	mkdir -p ./include/websocketpp

	mkdir -p ./include/jsonlib

	git -C ./ccwslib init

	git -C ./json init

	git -C ./ccwslib pull https://github.com/seantywork/websocketpp.git master 

	git -C ./json pull https://github.com/seantywork/json.git develop

	/bin/cp -rf ./ccwslib/websocketpp/* ./include/websocketpp

	/bin/cp -rf ./json/single_include/nlohmann/* ./include/jsonlib

	sudo rm -rf ./ccwslib ./json


solimod: $(SOLIMOD_OBJS)

	gcc $(GCC_LIB_FLAGS) $(INCLUDES) -o libsolimod.so $(SOLIMOD_OBJS)

	mv libsolimod.so ./src/modules

solimod.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod.o lib/solimod.c 


soliapp: 

	make -C src

clean:

	rm -rf *.o *.out *.so *.a src/soliapp src/*.o src/lib/*.so src/lib/*.a src/lib/*.o