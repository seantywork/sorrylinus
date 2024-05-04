

GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

DEP_PACKAGES := libssl-dev

INCLUDES := -I./include

LIBS := -lpthread -lssl -lcrypto 


OBJS := fhub_core.o
OBJS += fhub_sock_v1.o

all: 

	@echo "frank hub: dev, release"


dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dev: $(OBJS)

	gcc $(GCC_DEV_FLAGS) $(INCLUDES) -o hubc main.c $(OBJS) $(LIBS) 


release: $(OBJS)

	gcc $(GCC_REL_FLAGS) $(INCLUDES) -o hubc main.c $(OBJS) $(LIBS) 


fhub_core.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_core.o main_core.c 

fhub_sock_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o fhub_sock_v1.o sock/v1.c 

clean:

	rm -r *.o 

