

OUTPUT_DIR=$(shell pwd)

PROTOS := $(wildcard *.proto)
SRCS   := $(patsubst %.proto, $(OUTPUT_DIR)/%.pb.cc, $(PROTOS)) $(patsubst %.proto, $(OUTPUT_DIR)/%_pb2.py, $(PROTOS))


all : $(SRCS)
	
t :
	@echo $(OUTPUT_DIR)
	@echo $(PROTOS)
	@echo $(SRCS)

clean :
	rm -rf $(SRCS) *.pb.h *.pyc *.d *.h *.pb.o

$(OUTPUT_DIR)/%.pb.cc : %.proto
	/home/s/safe/bin/protoc3 --cpp_out=$(OUTPUT_DIR) $<
$(OUTPUT_DIR)/%_pb2.py : %.proto
	/home/s/safe/bin/protoc3 --python_out=$(OUTPUT_DIR) $<
