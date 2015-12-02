include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = lddstore.so

# Target specific variables
lddstore.so_SOURCES = $(wildcard *.cc) $(wildcard db/*.cc)
lddstore.so_CXXFLAGS= -I../../ -I/usr/local/include -I../../../lib/xleveldb/include
lddstore.so_LDFLAGS =  -L../util \
					-L../courier \
					-L../net \
					-L../protocol \
					-L../protocol/client \
					-L../protocol/server \
					-L/usr/local/lib \
					-L../../../lib/xleveldb
lddstore.so_LDLIBS  = -lglog -lgflags -llddnet -llddcourier -llddutil -llddprotocol_cli -llddprotocol_srv -lxleveldb
lddstore.so_RPATHS  = ../util \
					../courier \
					../net \
					../protocol \
					../protocol/client \
					../protocol/server \
					/usr/local/lib \
					../../../lib/xleveldb

# Variables affecting global compiler flags
#DEBUGING = -g -DDEBUG # Comment this line for non-debugging code
#OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

