PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror \
	-I../../../ \
	-I/usr/local/include \
	-I../../net/ \
	-I../../protocol/

LDFLAGS := -pthread \
	-L../../util \
	-L../../courier \
	-L../../net \
	-L../../protocol/server \
	-L/usr/local/lib/ \
	-L../../protocol/client

RTFLAGS := \
	-Wl,-rpath=$(PWD)/../../util \
	-Wl,-rpath=$(PWD)/../../courier \
	-Wl,-rpath=$(PWD)/../../net \
	-Wl,-rpath=$(PWD)/../../protocol/ \
	-Wl,-rpath=$(PWD)/../../protocol/client \
	-Wl,-rpath=/home/s/lib/ 

LIBS := -lglog -lgflags -llddnet -llddutil -llddcourier -llddprotocol_cli

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

TARGET := ut_proxy 

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

test: static
	$(MAKE) test -C ut

.PHONY: all target clean test static
