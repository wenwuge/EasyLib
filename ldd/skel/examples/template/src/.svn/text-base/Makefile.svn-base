include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = template.so

# Target specific variables
template.so_SOURCES = $(wildcard *.cc)
template.so_CXXFLAGS= 
template.so_LDFLAGS = 
template.so_LDLIBS  = -llddnet
template.so_RPATHS  = 
template.so_VERSION = 0.0.0

# Variables affecting global compiler flags
DEBUGING = -g # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = log template *.pid

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

