include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = client

LDDDIR=$(shell pwd)/../../../../../ldd

# Target specific variables
client_SOURCES = $(wildcard *.cc)
client_CXXFLAGS= -I$(LDDDIR)/../
client_LDFLAGS = -L$(LDDDIR)/net
client_LDLIBS  = -llddnet
client_RPATHS  = $(LDDDIR)/net

# Variables affecting global compiler flags
DEBUGING = -g # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Packaging options
SRCTOP = 
SRCTAR = yes # Comment this line if no src tarball is needed

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

