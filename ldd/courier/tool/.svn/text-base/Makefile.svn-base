




EXECUTABLE := ./create_zk_tree
INCS :=    
LIBPATHS := 



LDFLAGS := -pthread \
	-L../../util \
	-L/usr/local/lib/ \
	-L/usr/lib/
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
	
	
./create_zk_tree : create_zk_tree.cc 
	$(GXX) $(CFLAGS)  $(INCS) -lzookeeper_mt -Wno-strict-aliasing -o $@ $^  -lgflags -lpthread



clean:
	rm -f *.o *.a $(EXECUTABLE)
	
	
	
