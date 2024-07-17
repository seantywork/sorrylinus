
GCC_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

GCC_LIB_FLAGS := -Wall -shared

GCC_LIB_OBJ_FLAGS := -Wall -c -fpic 

DEP_PACKAGES := sqlite3 libsqlite3-dev libssl-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev

INCLUDES := -I../include

LINKS := -L../lib

LOADS := -Wl,-rpath=../lib

LIBS := -lsolimod -lsqlite3 -lssl -lcrypto

SOLIMOD_LIBS := -lrt -lpthread

# -lpgpio

OBJS := soli_core.o
OBJS += soli_sock_v1.o

SOLIMOD_OBJS := solimod_info_v1.o
SOLIMOD_OBJS += solimod_ir_v1.o
SOLIMOD_OBJS += solimod_cctv_v1.o

all:

	@echo "sorrylinus"

dep-package:


	apt-get update

	apt-get -y install $(DEP_PACKAGES)


v1: $(OBJS) solimod 

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o solic main.c $(OBJS) $(LIBS) 

soli_core.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_core.o main_core.c

soli_sock_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_sock_v1.o v1/sock/core.c


solimod: $(SOLIMOD_OBJS)

	gcc $(GCC_LIB_FLAGS) $(INCLUDES) -o ../lib/libsolimod.so $(SOLIMOD_OBJS) $(SOLIMOD_LIBS)


solimod_info_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_info_v1.o modules/v1/info/core.c

solimod_ir_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_ir_v1.o modules/v1/ir/core.c

solimod_cctv_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_cctv_v1.o modules/v1/cctv/core.c


clean:

	rm -r *.o *.out solic soliapp