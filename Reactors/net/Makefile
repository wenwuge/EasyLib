CXX=/usr/bin/g++
LIBS=-lpthread   
INCLUDE=-I./ -I../base/
CFLAGS=-g -D MUDUO_STD_STRING 
DIRS = $(shell find . -maxdepth 1 -type d)
FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
OBJS=$(patsubst %.cc,%.o,$(wildcard *.cc $(FILES)))

#TARGET=testthread
TARGET=libtcpserver.a

$(TARGET) : $(OBJS)
	echo $(OBJS)
	#g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	ar cr $@ $^
%.o : %.cc
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
