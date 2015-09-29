CXX=/usr/bin/g++
INCLUDE=-I/home/libin3-s/locallibevnet/include -I/usr/include/mysql
CFLAGS=-g 
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
TARGET=libcommon.a

$(TARGET) : $(OBJS)
	#g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	ar cr $@ $^ 
%.o : %.cpp
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
