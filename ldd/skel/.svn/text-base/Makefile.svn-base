include /home/s/share/topmk/topmk.def
# List of targets to be build
TARGETS = lddskel liblddskel.so

# Target specific variables
liblddskel.so_SOURCES = $(filter-out main.cc,$(wildcard *.cc)) $(wildcard internal/*.cc)
liblddskel.so_CXXFLAGS= -I../../
liblddskel.so_LDFLAGS = -L../net
liblddskel.so_LDLIBS  = -llddnet -lboost_program_options-mt -ldaemon -ldl
liblddskel.so_RPATHS  = ../net

lddskel_SOURCES  = main.cc
lddskel_CXXFLAGS = -I../../
lddskel_LDFLAGS  = -L.
lddskel_LDLIBS   = -llddskel
lddskel_RPATHS   = .

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

lddskel : liblddskel.so
