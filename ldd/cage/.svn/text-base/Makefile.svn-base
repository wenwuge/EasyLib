include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = liblddcage.so

# Target specific variables
liblddcage.so_SOURCES = $(wildcard *.cc) $(wildcard internal/*.cc) \
	$(wildcard recipes/*.cc)
liblddcage.so_CXXFLAGS= -I../../
liblddcage.so_LDFLAGS = -L../net -L../util
liblddcage.so_LDLIBS  = -lzookeeper_st -llddutil -llddnet
liblddcage.so_RPATHS  = ../net ../util

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

