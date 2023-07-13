ifeq (${USR_LOCAL_LIB_PREFIX},)
	USR_LOCAL_LIB_PREFIX := /usr/local/lib
endif

all: src/app.cc

	g++ -Wall -pthread -L${USR_LOCAL_LIB_PREFIX}/sorrylinus/test -Wl,-rpath=${USR_LOCAL_LIB_PREFIX}/sorrylinus/test src/app.cc -o src/soliapp -l sqlite3 -l soliapi_test

test: tools/test_soli.cc

	g++ -Wall -L${USR_LOCAL_LIB_PREFIX}/sorrylinus/test -Wl,-rpath=${USR_LOCAL_LIB_PREFIX}/sorrylinus/test  tools/test_soli.cc -o tools/testsoli -l soliapi_test 


clean: src/soliapp

	rm -r src/soliapp


testclean: tools/testsoli
	
	rm -r tools/testsoli 

