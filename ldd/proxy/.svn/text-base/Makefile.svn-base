# Configs
MODULE_NAME = lddproxy
MODULE_VERSION = 1.0.2

PWD := $(shell pwd)


# Templates
CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror -fPIC -DNDEBUG \
	-I../.. \
	-I/usr/local/include

LDFLAGS := -pthread \
	-L../util \
	-L../courier \
	-L../net \
	-L../protocol/ \
	-L../protocol/client \
	-L../protocol/server \
	-L/usr/local/lib/

RTFLAGS := \
	-Wl,-rpath=/home/s/lib/ \
	-Wl,-rpath=$(PWD)/../util \
	-Wl,-rpath=$(PWD)/../courier \
	-Wl,-rpath=$(PWD)/../net \
	-Wl,-rpath=$(PWD)/../protocol/ \
	-Wl,-rpath=$(PWD)/../protocol/client \
	-Wl,-rpath=$(PWD)/../protocol/server

LIBS := -lglog -lgflags \
	-llddutil \
	-llddnet \
	-llddcourier \
	-llddprotocol \
	-llddprotocol_cli \
	-llddprotocol_srv

DIR := . c2p p2s
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

LIBNAME = $(MODULE_NAME)
LIBVER = $(MODULE_VERSION)
SHARED_EXT = so
SHARED_LIB = $(LIBNAME).$(SHARED_EXT)

LIBVER_MAJOR = $(shell echo $(LIBVER) | cut -d. -f1)

all: shared

shared: $(SHARED_LIB).$(LIBVER) $(SHARED_LIB).$(LIBVER_MAJOR) $(SHARED_LIB)
	@echo "int main(){}" > .linking-test.cc \
		&& $(CXX) -o .linking-test .linking-test.cc $(SHARED_LIB); \
		export ret=$$?; \
		rm -f .linking-test .linking-test.cc; \
		test $$ret -ne 0 && rm -f $^; \
		exit $$ret

$(SHARED_LIB).$(LIBVER) : $(OBJ)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(SHARED_LIB).$(LIBVER_MAJOR)

$(SHARED_LIB).$(LIBVER_MAJOR) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

$(SHARED_LIB) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

-include $(DEP)

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.d : %.cc
	@$(CXX) -MM $< $(CXXFLAGS) | sed 's/$(notdir $*)\.o/$(subst /,\/,$*).o $(subst /,\/,$*).d/g' > $@

clean:
	-rm -rf $(TARGET) $(OBJ) $(DEP) *.a *.so *.so.* core.* *.core *.pid log

test: shared
	$(MAKE) test -C ut

.PHONY: all clean test shared

