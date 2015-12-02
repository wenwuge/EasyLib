PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror \
	-I../ \
	-I../../../ \

LDFLAGS := -pthread \
	-L../ \
	-L../../client \
	-L/usr/local/lib/
RTFLAGS := \
	-Wl,-rpath=$(PWD)/../ \
	-Wl,-rpath=$(PWD)/../../client \
	-Wl,-rpath=/home/s/lib/ 

	
LIBS := -lglog -lgflags -llddcli -llddcli_syn

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

STATIC_EXT = a
STATIC_LIB = $(LIBNAME).$(STATIC_EXT)

TARGET := ./ldd_bench

all: $(TARGET)

static: $(STATIC_LIB)

$(TARGET): ldd_store_bench.o $(STATIC_LIB)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS)

$(STATIC_LIB) : $(filter-out ldd_store_bench.o, $(OBJ))
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
