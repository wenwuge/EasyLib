PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror \
	-I../ \
	-I../../ \
	-I../../../ \
	-I/usr/local/include \
	-I../../protocol/client	\
	-I../../protocol/server
LDFLAGS := -pthread \
	-L../../net \
	-L../../protocol \
	-L../../protocol/client \
	-L../../protocol/server \
	-L/usr/local/lib/
RTFLAGS := \
	-Wl,-rpath=$(PWD)/../../net \
	-Wl,-rpath=$(PWD)/../../protocol \
	-Wl,-rpath=$(PWD)/../../protocol/client \
	-Wl,-rpath=$(PWD)/../../protocol/server \
	-Wl,-rpath=/home/s/lib/ 
	
LIBS := -lglog -lgflags -lgtest -llddnet  -llddprotocol -llddprotocol_srv -llddprotocol_cli

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

STATIC_EXT = a
STATIC_LIB = $(LIBNAME).$(STATIC_EXT)

TARGET := ./ut_store

all: $(TARGET)

static: $(STATIC_LIB)

$(TARGET): ut_store.o $(STATIC_LIB)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS)

$(STATIC_LIB) : $(filter-out ut_store.o, $(OBJ))
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
