
GCC_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

GCC_LIB_FLAGS := -Wall -shared

GCC_LIB_OBJ_FLAGS := -Wall -c -fpic 

GSTREAMER_PKG := libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

GSTREAMER_PKG_ARM := $(GSTREAMER_PKG) gstreamer1.0-libcamera

DEP_PACKAGES := sqlite3 libsqlite3-dev libssl-dev $(GSTREAMER_PKG)

DEP_PACKAGES_ARM := sqlite3 libsqlite3-dev libssl-dev $(GSTREAMER_PKG_ARM)

DEP_PIGPIO := git clone https://github.com/seantywork/pigpio.git

INCLUDES_ARC := -I/usr/include/x86_64-linux-gnu -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/aarch64-linux-gnu -I/usr/lib/aarch64-linux-gnu/glib-2.0/include  

INCLUDES := -I../include -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 $(INCLUDES_ARC)

# 

LINKS := -L../lib

LOADS := -Wl,-rpath=../lib

LIBS := -lsolimod -lsqlite3 -lssl -lcrypto 

SOLIMOD_LIBS := -lrt -lm -lpthread -lpigpio -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0

# -lpgpio

OBJS := soli_sock_v1.o
OBJS += soli_cmd_v1.o

SOLIMOD_OBJS := solimod_v1.o
SOLIMOD_OBJS += solimod_info_v1.o
SOLIMOD_OBJS += solimod_ir_v1.o
SOLIMOD_OBJS += solimod_cctv_v1.o


COMMON_OBJS := soli_utils.o 
COMMON_OBJS += soli_conf.o 

COMMON_MOD_OBJS := soli_utils_mod.o 
COMMON_MOD_OBJS += soli_conf_mod.o 


all:

	@echo "sorrylinus"

dep-package:


	apt-get update

	apt-get -y install $(DEP_PACKAGES)


dep-package-arm:

	apt-get update

	apt-get -y install $(DEP_PACKAGES_ARM)

dep-source:

	rm -rf pigpio && $(DEP_PIGPIO) && cd pigpio && make && make install && rm -rf pigpio


v1: $(OBJS) solimod $(COMMON_OBJS)

	gcc $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o solic main.c $(OBJS) $(COMMON_OBJS) $(LIBS) 


soli_sock_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_sock_v1.o v1/sock/core.c

soli_cmd_v1.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_cmd_v1.o v1/cmd/core.c


solimod: $(SOLIMOD_OBJS) $(COMMON_MOD_OBJS)

	gcc $(GCC_LIB_FLAGS) $(INCLUDES) -o ../lib/libsolimod.so $(SOLIMOD_OBJS) $(COMMON_MOD_OBJS) $(SOLIMOD_LIBS) 


solimod_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_v1.o modules/v1/core.c


solimod_info_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_info_v1.o modules/v1/info/core.c

solimod_ir_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_ir_v1.o modules/v1/ir/core.c

solimod_cctv_v1.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o solimod_cctv_v1.o modules/v1/cctv/core.c

soli_utils.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_utils.o utils/core.c

soli_conf.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o soli_conf.o utils/conf.c

soli_utils_mod.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o soli_utils_mod.o utils/core.c

soli_conf_mod.o:

	gcc $(GCC_LIB_OBJ_FLAGS) $(INCLUDES) -o soli_conf_mod.o utils/conf.c

clean:

	rm -r *.o *.out solic soliapp