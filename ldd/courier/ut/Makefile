




EXECUTABLE := ./ut_proxy ./ut_zkstation
INCS :=    
LIBPATHS := ../liblddcourier.a



LDFLAGS := -pthread \
	-L../../util \
	-L/usr/local/lib/
RTFLAGS := \
	-Wl,-rpath=/home/s/lib/ \
	-Wl,-rpath=$(PWD)/../../util
	

CFLAGS = -g -Wall  
GCC = gcc
GXX = g++

DIRS:=.
SOURCE := $(foreach DIRS,$(DIRS),$(wildcard $(DIRS)/*.cc))
OBJS := $(patsubst %.cc,%.o,$(SOURCE))

all: $(EXECUTABLE)
	
./ut_proxy : ut_proxy.cc ut_common.o  $(LIBPATHS) 
	$(GXX) $(CFLAGS) $(INCS) -Wno-strict-aliasing -lzookeeper_st -o $@ $^ $(RTFLAGS) $(LDFLAGS) -lglog -lgtest -lpthread -llddutil -lgflags

./ut_zkstation : ut_zkstation.cc ut_common.o -lpthread
	$(GXX) $(CFLAGS)  $(INCS) -lzookeeper_mt -Wno-strict-aliasing -o $@ $^  -lgflags

ut_common.o : ut_common.cc
	$(GXX) $(CFLAGS)  -c $^
	


clean:
	rm -f *.o *.a $(EXECUTABLE)
	
	
	
