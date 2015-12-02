include /home/s/share/topmk/topmk.def

# Root directory
ROOT = .

# List of targets to be build
TARGETS = test

# Variables affecting global compiler flags
DEBUGING = -g # Comment this line for non-debugging code
OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Variables for specific target
test_SOURCES = $(wildcard *.cc)
test_CXXFLAGS= -I ../../
test_LDFLAGS = -L ../../
test_LDLIBS  = -llddnet -lglog
test_RPATHS  = ../../

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

