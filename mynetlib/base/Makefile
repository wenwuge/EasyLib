CXX=/usr/bin/g++
LIBS=-lpthread   
INCLUDE=-I./
CFLAGS=-g 
OBJS=$(patsubst %.cc,%.o,$(wildcard *.cc))
#TARGET=testthread
TARGET=libbase.a

$(TARGET) : $(OBJS)
	#g++ -o $@ $^ $(LIBS) $(CFLAGS) $(INCLUDE) 
	ar cr $@ $^
%.o : %.cc
	g++ -c  $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(TARGET)
