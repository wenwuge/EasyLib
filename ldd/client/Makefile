PWD := $(shell pwd)

CXX := g++
CXXFLAGS := -g3 -O0 -Wall -Werror -fPIC -DNDEBUG \
	-I../../ \
	-I/usr/local/include

LDFLAGS := -pthread \
	-L../util \
	-L../net \
	-L../courier \
	-L../protocol/ \
	-L../protocol/client \
	-L../protocol/server \
	-L/usr/local/lib

RTFLAGS := \
	-Wl,-rpath=$(PWD)/../util \
	-Wl,-rpath=$(PWD)/../protocol/ \
	-Wl,-rpath=$(PWD)/../protocol/client \
	-Wl,-rpath=$(PWD)/../protocol/server \
	-Wl,-rpath=$(PWD)/../net/ \
	-Wl,-rpath=$(PWD)/../courier \
	-Wl,-rpath=/home/s/lib

LIBS := -lglog \
	-llddutil \
	-llddnet \
	-llddcourier \
	-llddprotocol \
	-llddprotocol_cli\
	-llddprotocol_srv

DIR := . ./internal
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

LIBNAME = lddcli
LIBVER = 1.0.2

SHARED_EXT = so
STATIC_EXT = a
SHARED_LIB = lib$(LIBNAME).$(SHARED_EXT)
STATIC_LIB = lib$(LIBNAME).$(STATIC_EXT)

LIBVER_MAJOR = $(shell echo $(LIBVER) | cut -d. -f1)

all: shared static

shared: $(SHARED_LIB).$(LIBVER) $(SHARED_LIB).$(LIBVER_MAJOR) $(SHARED_LIB)
	@echo "int main(){}" > .linking-test.cc \
		&& $(CXX) -o .linking-test .linking-test.cc -L. -l$(LIBNAME); \
		export ret=$$?; \
		rm -f .linking-test .linking-test.cc; \
		test $$ret -ne 0 && rm -f $^; \
		exit $$ret

static: $(STATIC_LIB)

$(SHARED_LIB).$(LIBVER) : $(OBJ)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(SHARED_LIB).$(LIBVER_MAJOR)

$(SHARED_LIB).$(LIBVER_MAJOR) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

$(SHARED_LIB) : $(SHARED_LIB).$(LIBVER)
	ln -sf $^ $@

$(STATIC_LIB) : $(OBJ)
	ar -rs $@ $^

-include $(DEP)

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.d : %.cc
	@$(CXX) -MM $< $(CXXFLAGS) | sed 's/$(notdir $*)\.o/$(subst /,\/,$*).o $(subst /,\/,$*).d/g' > $@

clean:
	-rm -rf $(OBJ) $(DEP) *.a *.so *.so.*

test: shared
	$(MAKE) test -C ut

.PHONY: all clean test shared static

