include /home/s/share/topmk/topmk.def

# List of executable files to be build
EXECUTABLE_FILES = test
# List of shared libraries to be build
SHARED_LIBRARIES = 
# List of static libraries to be build
STATIC_LIBRARIES = 

# Target specific variables
test_SOURCES = $(wildcard *.cc)
test_CXXFLAGS= -I../../../
test_LDFLAGS = -L../ -L../../net -L../../util
test_LDLIBS  = -llddcage -llddnet -llddutil
test_RPATHS  = ../ ../../net ../../util

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

