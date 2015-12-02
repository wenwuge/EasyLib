# If this is a directory with subdirectories to build, set SUBDIRS
SUBDIRS = client courier net protocol proxy skel store util syn_client cage

# Do not add any build rules until *after* the include of topmk.all
include /home/s/share/topmk/topmk.all
# Do not place any rules or targets above this line

courier net protocol : util
client proxy store: net protocol courier
proxy store: skel
cage skel: net
syn_client : client

