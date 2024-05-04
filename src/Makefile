
GCC_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

GCC_LIB_FLAGS := -Wall -shared

GCC_LIB_OBJ_FLAGS := -Wall -c -fpic 

DEP_PACKAGES := sqlite3 libsqlite3-dev libssl-dev

INCLUDES := -I../include

LINKS := -L../lib

LOADS := -Wl,-rpath=../lib

LIBS := -lsolimod -lsqlite3 -lssl -lcrypto

SOLIMOD_LIBS := -lrt -lpthread

# -lpgpio

OBJS := soli_core.o
OBJS += soli_sock_v1.o

SOLIMOD_OBJS := solimod_info_v1.o

all:

	@echo "sorrylinus"

dep-package:


	apt-get update

	apt-get -y install $(DEP_PACKAGES)


v1: $(OBJS) solimod 

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o soliapp main.c $(OBJS) $(LIBS) 

soli_core.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_core.o main_core.c

soli_sock_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_sock_v1.o sock/v1.c


solimod: $(SOLIMOD_OBJS)

	gcc $(GCC_LIB_FLAGS) $(INCLUDES) -o ../lib/libsolimod.so $(SOLIMOD_OBJS) $(SOLIMOD_LIBS)


solimod_info_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_info_v1.o modules/info/v1.c


clean:

	rm -r *.o 