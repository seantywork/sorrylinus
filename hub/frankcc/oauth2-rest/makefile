SRC = 	./main.cpp\
		./vendor/properties_file_reader.cpp\
		./vendor/json/jsoncpp.cpp
		
OBJ = $(SRC:.cpp=.o)
OUTDIR = .
INCLUDES = 	-I. -I./vendor
			
CCFLAGS = -Wall -g
CCC = g++
LDLIBS= -l:librestclient-cpp.a -lcurl

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

default: restclient
	
all: clean restclient
	
restclient: $(OBJ)
	$(CCC) -o $(OUTDIR)/restclient $(OBJ) $(LDLIBS)

clean:
	rm -f $(OBJ) $(OUTDIR)/restclient
