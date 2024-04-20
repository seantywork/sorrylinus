
GCC_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c


INCLUDES := -I../include

LINKS := -L./lib

LOADS := -Wl,-rpath=./lib

LIBS := -lsolimod -lsqlite3 -lssl -lcrypto


APP_OBJS := client.o 
APP_OBJS += client_mod.o
APP_OBJS += models.o





all: $(APP_OBJS)

	g++ $(GCC_FLAGS) $(INCLUDES) $(LINKS) $(LOADS) -o soliapp app.cc $(APP_OBJS) $(LIBS) 


client.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o client.o sock/client.cc 



client_mod.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o client_mod.o sock/client_mod.cc 



models.o:

	g++ $(GCC_OBJ_FLAGS) $(INCLUDES) -o models.o models/models.cc 
