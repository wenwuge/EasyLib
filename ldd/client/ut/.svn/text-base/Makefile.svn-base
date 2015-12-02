PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror -fPIC \
	-I../../../../ \
	-I../../../ \
	-I../../ \
	-I/usr/local/include  
	
	
LDFLAGS := -pthread \
	-L/usr/local/lib \
	-L../../util \
	-L../../net \
	-L../../courier \
	-L../../protocol/ \
	-L../../protocol/client \
	-L../../protocol/server \
	-L../../client
	

RTFLAGS := \
	-Wl,-rpath=/home/s/lib \
	-Wl,-rpath=$(PWD)/../../util \
	-Wl,-rpath=$(PWD)/../../protocol/ \
	-Wl,-rpath=$(PWD)/../../protocol/client \
	-Wl,-rpath=$(PWD)/../../protocol/server \
	-Wl,-rpath=$(PWD)/../../net/ \
	-Wl,-rpath=$(PWD)/../../courier \
	-Wl,-rpath=$(PWD)/../../client
	
RTFLAGS := \
	-Wl,-rpath=/home/s/lib \
	-Wl,-rpath=$(PWD)/../../util \
	-Wl,-rpath=$(PWD)/../../protocol/ \
	-Wl,-rpath=$(PWD)/../../protocol/client \
	-Wl,-rpath=$(PWD)/../../protocol/server \
	-Wl,-rpath=$(PWD)/../../net/ \
	-Wl,-rpath=$(PWD)/../../courier \
	-Wl,-rpath=$(PWD)/../../client
	
LIBS := -lglog \
	-llddutil \
	-llddnet \
	-llddcourier \
	-llddprotocol \
	-llddprotocol_cli \
	-llddprotocol_srv \
	-llddcli

DIR := . db
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

STATIC_EXT = a
STATIC_LIB = $(LIBNAME).$(STATIC_EXT)

TARGET := ut_client

all: $(TARGET)

static: $(STATIC_LIB)

$(TARGET): ut_client.o $(STATIC_LIB)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS)

$(STATIC_LIB) : $(filter-out main.o, $(OBJ))
	ar -rs $@ $^

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
