#TOPMK=/home/s/share/topmk
TOPMK=$(shell while [ ! -f topmk/topmk.all ] && [ `pwd` != / ] ; do cd ..; done ; echo `pwd`/topmk)
include $(TOPMK)/topmk.def
# Root directory
ROOT = .

# List of target files to be build
TARGETS = liblddutil.so

# Target specific variables
liblddutil.so_HEADERS = $(wildcard *.h) $(wildcard *.inl) $(wildcard detail/atomic/*.h)
liblddutil.so_HPREFIX = ldd/util
liblddutil.so_SOURCES = $(wildcard *.cc) $(wildcard detail/atomic/*.cc)
liblddutil.so_CXXFLAGS= -I../../
liblddutil.so_LDFLAGS =
liblddutil.so_LDLIBS  = -lglog -lrt
liblddutil.so_RPATHS  =

# Variables affecting global compiler flags
#DEBUGING = -g # Comment this line for non-debugging code
#OPTIMIZE = -O0 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include $(TOPMK)/topmk.all
# Do not place any rules or targets above this line

