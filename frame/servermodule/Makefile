Target=libservermodule.so
CXX=/usr/bin/g++
Cflag=-shared -fPIC
Objs=$(patsubst %.cpp , %.o, $(wildcard *.cpp))
Include=-I../../include/
LIBS= -Wl,--whole-archive ../../net/libtcpserver.a -Wl,--no-whole-archive
$(Target) :$(Objs)
	$(CXX) -o $(Target) $^ $(Cflag) $(LIBS)	

%.o : %.cpp
	$(CXX) -C $^ $(Include)

clean:
	rm -f *.o *.so
