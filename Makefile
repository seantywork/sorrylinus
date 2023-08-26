ifeq (${USR_LOCAL_INCLUDE_PREFIX},)
	USR_LOCAL_INCLUDE_PREFIX := /usr/local/include
endif


ifeq (${USR_LOCAL_LIB_PREFIX},)
	USR_LOCAL_LIB_PREFIX := /usr/local/lib
endif


ifeq (${USR_LOCAL_INCLUDE_SOLIMOD},)
	USR_LOCAL_INCLUDE_SOLIMOD := /usr/local/include/sorrylinusmod
endif

ifeq (${USR_LOCAL_LIB_SOLIMOD},)
	USR_LOCAL_LIB_SOLIMOD := /usr/local/lib/sorrylinusmod
endif


all: src/app.cc

	g++ -Wall -pthread -L${USR_LOCAL_LIB_SOLIMOD}/mod -Wl,-rpath=${USR_LOCAL_LIB_SOLIMOD}/mod -o src/soliapp src/app.cc -lsqlite3 -lsolimod


dep-install:

	cp -R include/httplib ${USR_LOCAL_INCLUDE_PREFIX}/
	cp -R include/jsonlib ${USR_LOCAL_INCLUDE_PREFIX}/
	cp -R include/websocketpp ${USR_LOCAL_INCLUDE_PREFIX}/

	chmod -R 655 ${USR_LOCAL_INCLUDE_PREFIX}/httplib
	chmod -R 655 ${USR_LOCAL_INCLUDE_PREFIX}/jsonlib
	chmod -R 655 ${USR_LOCAL_INCLUDE_PREFIX}/websocketpp


dep-uninstall:

	rm -r ${USR_LOCAL_INCLUDE_PREFIX}/httplib
	rm -r ${USR_LOCAL_INCLUDE_PREFIX}/jsonlib
	rm -r ${USR_LOCAL_INCLUDE_PREFIX}/websocketpp

dep-install-lib:

	sudo apt-get install -y libboost-all-dev


dep-install-solimod:

	mkdir -p ${USR_LOCAL_INCLUDE_SOLIMOD}
	mkdir -p ${USR_LOCAL_LIB_SOLIMOD}
	cp -R include/sorrylinusmod/* ${USR_LOCAL_INCLUDE_SOLIMOD}/
	chmod -R 655 ${USR_LOCAL_INCLUDE_SOLIMOD}
	chmod -R 655 ${USR_LOCAL_LIB_SOLIMOD}

dep-uninstall-solimod:

	rm -r ${USR_LOCAL_INCLUDE_SOLIMOD}
	rm -r ${USR_LOCAL_LIB_SOLIMOD}


build: src/app.cc

	g++ -Wall -pthread -L${USR_LOCAL_LIB_SOLIMOD}/mod -Wl,-rpath=${USR_LOCAL_LIB_SOLIMOD}/mod -o src/soliapp src/app.cc -lsqlite3 -lsolimod


clean: src/soliapp

	rm -r src/soliapp


mod-build: lib/sorrylinusmod/mod/solimod.c

	gcc -c -Wall -o solimod.o -fpic lib/sorrylinusmod/mod/solimod.c

	gcc -shared -o libsolimod.so solimod.o

mod-clean: solimod.o libsolimod.so

	rm -r solimod.o
	rm -r libsolimod.so

mod-install: libsolimod.so

	install -d ${USR_LOCAL_LIB_SOLIMOD}/mod
	install -m 644 libsolimod.so ${USR_LOCAL_LIB_SOLIMOD}/mod

mod-uninstall:

	rm -r ${USR_LOCAL_LIB_SOLIMOD}/mod


test-build: test/test_soli.cc

	g++ -Wall -L${USR_LOCAL_LIB_SOLIMOD}/test -Wl,-rpath=${USR_LOCAL_LIB_SOLIMOD}/test -o test/testsoli test/test_soli.cc -lsolimod_test 


test-clean: test/testsoli
	
	rm -r test/testsoli

test-mod-build: lib/sorrylinusmod/test/solimod_test.c

	gcc -c -Wall -o solimod_test.o -fpic lib/sorrylinusmod/test/solimod_test.c

	gcc -shared -o libsolimod_test.so solimod_test.o

test-mod-clean: solimod_test.o libsolimod_test.so
	
	rm -r solimod_test.o
	rm -r libsolimod_test.so

test-mod-link: libsolimod_test.so

	install -d ${USR_LOCAL_LIB_SOLIMOD}/test
	install -m 644 libsolimod_test.so ${USR_LOCAL_LIB_SOLIMOD}/test


test-mod-unlink: 

	rm -r ${USR_LOCAL_LIB_SOLIMOD}/test
