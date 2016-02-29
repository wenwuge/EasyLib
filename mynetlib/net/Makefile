CXX=/usr/bin/g++
LIBS=-lpthread   
INCLUDE=-I./ -I../base/
CFLAGS=-g 
DIR := poller .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJS := $(patsubst %.cc, .obj/%.o, $(SRC))

#TARGET=testthread
TARGET=libnet.a
ALL: 
	make prepare
	make $(TARGET)
prepare:
	mkdir -p .obj/poller
$(TARGET) : $(OBJS)
	#g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	ar cr $@ $^
.obj/%.o : %.cc
	g++ -c -o $@  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
