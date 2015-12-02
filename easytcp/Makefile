CXX=/usr/bin/g++
INCLUDE=-I/home/libin3-s/locallibevnet/include
LIBS=-lpthread  /home/libin3-s/locallibevnet/lib/libevent.a  
CFLAGS=-g -fPIC 
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
#TARGET=tcpserver
TARGET=libtcpserver.a

$(TARGET) : $(OBJS)
	ar cr $@ $^
%.o : %.cpp
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
