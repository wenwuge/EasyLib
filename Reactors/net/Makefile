CXX=/usr/bin/g++
LIBS=-lpthread   
INCLUDE=-I./ -I../base/
CFLAGS=-g -D MUDUO_STD_STRING 
DIRS = pollers ./
SRC := $(foreach d, $(DIRS), $(wildcard $(d)/*.cc))
OBJS:= $(patsubst %.cc, obj/%.o, $(SRC))


#DIRS = $(shell find . -maxdepth 1 -type d)
#FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
#OBJS=$(patsubst %.cc,%.o,$(wildcard *.cc $(FILES)))

#TARGET=testthread
TARGET=libtcpserver.a
ALL: prepare
	make $(TARGET)
prepare:
	mkdir -p obj
	mkdir -p obj/poller

$(TARGET) : $(OBJS)
	echo $(OBJS)
	#g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	ar cr $@ $^
obj/%.o : %.cc
	g++ -c  $^  -o $@  $(CFLAGS) $(INCLUDE)

clean:
	rm -rf obj
	rm -f $(TARGET)
