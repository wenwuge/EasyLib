PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror \
	-I../../../../ \
	-I/usr/include/boost141 \
	-I/usr/local/include
LDFLAGS := -pthread \
	-L../../../net \
	-L/usr/lib64/boost141 \
	-L/usr/local/lib/ \
	-lutil
RTFLAGS := \
	-Wl,-rpath=/home/s/lib/ \
	-Wl,-rpath=$(PWD)/../../../util \
	-Wl,-rpath=$(PWD)/../../../net
LIBS := -lglog -lgflags

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

TARGET := atomic_test

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
