CXX=/usr/bin/g++
INCLUDE=-I/home/libin3-s/locallibevnet/include -I../include/
LIBS=-lpthread  /home/libin3-s/locallibevnet/lib/libevent.a -Wl,-Bstatic  -llog4cxx -L../lib/log4cxx -Wl,-Bdynamic -lm -L/usr/lib64 -L/usr/local/apr/lib/ -lapr-1 -L/usr/local/apr-util/lib/ -laprutil-1 ../common/libcommon.a ../thread/libthread.a 
CFLAGS=-g -ldl -rdynamic 
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
TARGET=testhttp

$(TARGET) : $(OBJS)
	g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
%.o : %.cpp
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
