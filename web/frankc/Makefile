

GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

DEP_PACKAGES := libssl-dev

INCLUDES := -I./include

LIBS := -lpthread 


OBJS := f_admin.o
OBJS += 

all: 

	@echo "frank: dev, release"


dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dev: $(OBJS)

	gcc $(GCC_DEV_FLAGS) $(INCLUDES) -o frankc.out main.c $(OBJS) $(LIBS) 


release: $(OBJS)

	gcc $(GCC_REL_FLAGS) $(INCLUDES) -o frankc.out main.c $(OBJS) $(LIBS) 


f_admin.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o f_admin.o main_admin.c 


clean:

	rm -r *.o *.out

