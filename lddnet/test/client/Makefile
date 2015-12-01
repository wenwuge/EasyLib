PWD := $(shell pwd)
LDD := $(PWD)/../../../../ldd

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror \
	-I$(LDD)/../ \
	-I/usr/local/include
LDFLAGS := -pthread \
	-L$(LDD)/net \
	-L/usr/local/lib/
RTFLAGS := \
	-Wl,-rpath=$(LDD)/util \
	-Wl,-rpath=$(LDD)/net \
	-Wl,-rpath=/home/s/lib/

LIBS := -lglog -lgflags \
	-llddnet

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

TARGET := ldd_client

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS)

target: $(TARGET)

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d : %.cc
	@$(CXX) -MM $< $(CXXFLAGS) | sed 's/$(notdir $*)\.o/$(subst /,\/,$*).o $(subst /,\/,$*).d/g' > $@
%.d : %.cpp
	@$(CXX) -MM $< $(CXXFLAGS) | sed 's/$(notdir $*)\.o/$(subst /,\/,$*).o $(subst /,\/,$*).d/g' > $@

-include $(DEP)

clean:
	-rm -rf $(OBJ) $(TARGET) $(STATIC_LIB) $(DEP)
	-rm -rf *.pid *.log *.core

test: 
	$(MAKE) test -C ut

.PHONY: all target clean test static
