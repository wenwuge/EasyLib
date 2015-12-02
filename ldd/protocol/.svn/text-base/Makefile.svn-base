include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = liblddprotocol.so

## Target-specific variables
liblddprotocol.so_SOURCES = $(wildcard *.cc)
liblddprotocol.so_CXXFLAGS= -I../../
liblddprotocol.so_LDFLAGS = -L../util
liblddprotocol.so_LDLIBS  = -llddutil -lsnappy
liblddprotocol.so_RPATHS  = ../util

# Variables affecting global compiler flags
#DEBUGING = -g -DDEBUG # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = server client

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

client server : liblddprotocol.so
