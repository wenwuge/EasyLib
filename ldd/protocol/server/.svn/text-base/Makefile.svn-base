include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = liblddprotocol_srv.so

# Target specific variables
liblddprotocol_srv.so_SOURCES = $(wildcard *.cc)
liblddprotocol_srv.so_CXXFLAGS= -I../../../
liblddprotocol_srv.so_LDFLAGS = -L../ -L../../util
liblddprotocol_srv.so_LDLIBS  = -llddprotocol -lsnappy -llddutil
liblddprotocol_srv.so_RPATHS  = ../ ../../util

# Variables affecting global compiler flags
DEBUGING = -g -DDEBUG # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

