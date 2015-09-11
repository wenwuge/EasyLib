CXX=/usr/bin/g++
INCLUDE=-I/home/libin3-s/locallibevnet/include
LIBS=-lpthread  /home/libin3-s/locallibevnet/lib/libevent.a  
CFLAGS=-g 
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
TARGET=tcpserver

$(TARGET) : $(OBJS)
	g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
%.o : %.cpp
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
