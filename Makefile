ifeq (${USR_LOCAL_INCLUDE_PREFIX},)
	USR_LOCAL_INCLUDE_PREFIX := /usr/local/include
endif

ifeq (${USR_LOCAL_LIB_PREFIX},)
	USR_LOCAL_LIB_PREFIX := /usr/local/lib
endif


build: src/app.cc

	g++ -Wall -pthread -L${USR_LOCAL_LIB_PREFIX}/sorrylinus/test -Wl,-rpath=${USR_LOCAL_LIB_PREFIX}/sorrylinus/test src/app.cc -o src/soliapp -l sqlite3 -l soliapi_test

clean: src/soliapp

	rm -r src/soliapp


test-build: tools/test_soli.cc

	g++ -Wall -L${USR_LOCAL_LIB_PREFIX}/sorrylinus/test -Wl,-rpath=${USR_LOCAL_LIB_PREFIX}/sorrylinus/test  tools/test_soli.cc -o tools/testsoli -l soliapi_test 


test-clean: tools/testsoli
	
	rm -r tools/testsoli 

test-lam-build: lib/test/soliapi_test.cc

	g++ -fPIC -c -Wall lib/test/soliapi_test.cc -o soliapi_test.o

	g++ -shared soliapi_test.o -o libsoliapi_test.so


test-lam-install: libsoliapi_test.so

	install -d ${USR_LOCAL_INCLUDE_PREFIX}/sorrylinus/test
	install -m 644 include/test/soliapi_test.h ${USR_LOCAL_INCLUDE_PREFIX}/sorrylinus/test

	install -d ${USR_LOCAL_LIB_PREFIX}/sorrylinus/test
	install -m 644 libsoliapi_test.so ${USR_LOCAL_LIB_PREFIX}/sorrylinus/test

test-lam-clean: soliapi_test.o libsoliapi_test.so
	
	rm -r soliapi_test.o
	rm -r libsoliapi_test.so

test-lam-uninstall: 

	rm -r ${USR_LOCAL_INCLUDE_PREFIX}/sorrylinus/test
	rm -r ${USR_LOCAL_LIB_PREFIX}/sorrylinus/test
