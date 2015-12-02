




EXECUTABLE := ./main
INCS := -I../ -I../server/  -I../../../ -I../client
LIBPATHS := ../client/liblddprotocol_cli.a ../server/liblddprotocol_srv.a  ../liblddprotocol.a




CFLAGS = -g -Wall  
GCC = gcc
GXX = g++

DIRS:=.
SOURCE := $(foreach DIRS,$(DIRS),$(wildcard $(DIRS)/*.cc))
OBJS := $(patsubst %.cc,%.o,$(SOURCE))

all: $(EXECUTABLE)
	
./main : main.cc  $(LIBPATHS) 
	$(GXX) $(CFLAGS) $(INCS) -Wno-strict-aliasing  -o $@ $^ -lglog -lgtest -lpthread -lsnappy


clean:
	rm -f *.o *.a $(EXECUTABLE)
	
	
	
