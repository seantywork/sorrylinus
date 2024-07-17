

GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

DEP_PACKAGES := libssl-dev

DEP_MONGOOSE := git clone https://github.com/seantywork/mongoose.git

DEP_CJSON := git clone https://github.com/seantywork/cJSON.git

INCLUDES := -I./include -I./vendor -I./vendor/mongoose -I./vendor/cJSON

LIBS := -lpthread -lssl -lcrypto 



OBJS := fhub_core.o
OBJS += fhub_utils.o
OBJS += fhub_sock.o
OBJS += fhub_front.o

INCLUDESV1 := -I./include

LIBSV1 := -lpthread -lssl -lcrypto 

OBJSV1 := fhub_core_v1.o
OBJSV1 += fhub_admin_v1.o
OBJSV1 += fhub_utils_v1.o
OBJSV1 += fhub_sock_v1.o
OBJSV1 += fhub_front_v1.o

DEP_OBJS := mongoose.o
DEP_OBJS += cJSON.o

all: 

	@echo "frank hub: dev, release"


dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dep-source:

	cd vendor && rm -rf mongoose && $(DEP_MONGOOSE)

	cd vendor && rm -rf cJSON && $(DEP_CJSON)


dev: $(OBJS) $(DEP_OBJS)

	gcc $(GCC_DEV_FLAGS) $(INCLUDES) -o hubc.out main.c $(OBJS) $(DEP_OBJS) $(LIBS) 


release: $(OBJS) $(DEP_OBJS)

	gcc $(GCC_REL_FLAGS) $(INCLUDES) -o hubc.out main.c $(OBJS) $(DEP_OBJS) $(LIBS) 




fhub_core.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_core.o v2/main_core.c 

fhub_utils.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_utils.o v2/main_utils.c 

fhub_sock.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_sock.o v2/hubsock/core.c 

fhub_front.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_front.o v2/hubfront/core.c 


dep-package-v1:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dev-v1: $(OBJSV1)

	gcc $(GCC_DEV_FLAGS) $(INCLUDESV1) -o hubc_v1.out main_v1.c $(OBJSV1) $(LIBSV1) 

release-v1: $(OBJSV1)

	gcc $(GCC_REL_FLAGS) $(INCLUDESV1) -o hubc_v1.out main_v1.c $(OBJSV1) $(LIBSV1) 

fhub_core_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDESV1) -o fhub_core_v1.o v1/main_core.c 

fhub_admin_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDESV1) -o fhub_admin_v1.o v1/main_admin.c 

fhub_utils_v1.o:


	gcc $(GCC_OBJ_FLAGS) $(INCLUDESV1) -o fhub_utils_v1.o v1/main_utils.c 


fhub_sock_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDESV1) -o fhub_sock_v1.o v1/hubsock/core.c 

fhub_front_v1.o:


	gcc $(GCC_OBJ_FLAGS) $(INCLUDESV1) -o fhub_front_v1.o v1/hubfront/core.c 



mongoose.o:


	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o mongoose.o vendor/mongoose/mongoose.c 

cJSON.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o cJSON.o vendor/cJSON/cJSON.c 


clean:

	rm -r *.o *.out

