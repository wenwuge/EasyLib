include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = example2.so

LDDDIR=$(shell pwd)/../../../../../ldd

# Target specific variables
example2.so_SOURCES = $(wildcard *.cc)
example2.so_CXXFLAGS= -I$(LDDDIR)/../
example2.so_LDFLAGS = -L$(LDDDIR)/net
example2.so_LDLIBS  = -llddnet
example2.so_RPATHS  = $(LDDDIR)/net
example2.so_VERSION = 1.0.0

# Variables affecting global compiler flags
DEBUGING = -g # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = log *.pid example2

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

