Target=libechomodule.so
CXX=/usr/bin/g++
Cflag=-shared -fPIC
Objs=$(patsubst %.cpp , %.o, $(wildcard *.cpp))
Include=-I../../include/
$(Target) :$(Objs)
	$(CXX) -o $(Target) $^ $(Cflag) 	

%.o : %.cpp
	$(CXX) -C $^ $(Include)

clean:
	rm -f *.o *.so
