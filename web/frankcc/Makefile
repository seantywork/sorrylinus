
GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c


DEP_PACKAGES := libmysqlcppconn-dev libssl-dev libcurl4-openssl-dev


INCLUDES := -I./include

LIBS := -lpthread -lcurl -lmysqlcppconn 

OBJS := router.o
OBJS += controller.o 
OBJS += controller_modules.o 
OBJS += models.o


all:


	@echo "frankcc: dev, release"

dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)


dep-source:

	mkdir -p cchttplib

	mkdir -p json

	mkdir -p ./include/httplib

	mkdir -p ./include/jsonlib

	git -C ./cchttplib init

	git -C ./json init

	git -C ./cchttplib pull https://github.com/seantywork/cpp-httplib.git master 

	git -C ./json pull https://github.com/seantywork/json.git develop

	/bin/cp -rf ./cchttplib/httplib.h ./include/httplib/httplib.h

	/bin/cp -rf ./json/single_include/nlohmann/* ./include/jsonlib

	sudo rm -rf ./cchttplib ./json


dev: $(OBJS)

	g++ $(GCC_DEV_FLAGS) $(INCLUDES) -o frank main.cc $(OBJS) $(LIBS) 


release: $(OBJS)

	g++ $(GCC_REL_FLAGS) $(INCLUDES) -o frank main.cc $(OBJS) $(LIBS) 


router.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o router.o frouter/frouter.cc 


controller.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o controller.o fcontroller/fcontroller.cc 

controller_modules.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o controller_modules.o fcontroller/modules.cc 

models.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o models.o fmodels/fmodels.cc 

clean:

	rm -r *.o *.out frank