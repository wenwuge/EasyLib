CXX=/usr/bin/g++
INCLUDE=-I../include/
LIBS=-lpthread ../lib/zookeeper/libzookeeper_mt.a
CFLAGS=-g
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
TARGET=testzookeeper

$(TARGET) : $(OBJS)
	g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	#ar cr $@ $^
%.o : %.cpp
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
