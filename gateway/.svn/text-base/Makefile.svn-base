TOPMK=/home/s/include/topmk
include $(TOPMK)/topmk.def

# Root directory
ROOT = .

# List of targets to be build
TARGETS = lcs_gateway.so

# Variables affecting global compiler flags
DEBUGING = -DDEBUG # Comment this line for non-debugging code
OPTIMIZE = -DH_USING_PB3 # Comment this line for optimized(-O3) code
WARNINGS+= -Werror # Comment this line if warning is a problem

# Variables for specific target
lcs_gateway.so_SOURCES = $(wildcard *.cc) $(wildcard proto/*.cc)

lcs_gateway.so_CXXFLAGS= -I./ \
					-I/home/s/safe/include/protobuf3 \
					-I/home/s/safe/include \
					-I/home/s/include 

lcs_gateway.so_LDFLAGS = -L/home/s/safe/protobuf3/lib \
					-L/home/s/safe/lib \
					-L/home/s/lib 

lcs_gateway.so_LDLIBS  = -llddnet \
					-lprotobuf \
					-llddcage \
					-lqlog \
					-lmxhttp-event2 \
					-lqoslib \
					-lsafeutil_pb3 \
					#-lprofiler
					#-ltcmalloc \

lcs_gateway.so_RPATHS  = /home/s/safe/protobuf3/lib /home/s/safe/lib /home/s/lib 

# Optional local files to be removed when making 'clean'
# Objects and libraries are taken care of, they do *not* need to be set here
CLEAN = 

# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = 

# Do not add any build rules until *after* the include of topmk.all
include $(TOPMK)/topmk.all
# Do not place any rules or targets above this line

#proto:
#	make clean -C ./proto
#	make -C ./proto
#
#$(lcs_gateway.so_DEPENDS): proto

