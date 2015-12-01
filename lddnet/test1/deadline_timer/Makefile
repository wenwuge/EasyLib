CXX := g++ 
CXXFLAGS := -g3 -O0 -Wall -Werror -fPIC \
	-I/usr/include/boost141 \
	-I/usr/local/include

LDFLAGS := -L/usr/lib64/boost141 \
	-L/usr/local/lib

LIBS := -lglog -lboost_system

timer_test: main.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

