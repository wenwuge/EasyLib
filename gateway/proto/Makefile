OUTPUT_DIR=$(shell pwd)
PROTOC := /home/s/safe/bin/protoc3
PB2_OUTPUT_DIR=$(shell pwd)/pb2_output
PB2_PROTOS := $(wildcard *.proto)
PB2_SRCS   := $(patsubst %.proto, $(PB2_OUTPUT_DIR)/%.pb.cc, $(PB2_PROTOS)) $(patsubst %.proto, $(PB2_OUTPUT_DIR)/%_pb2.py, $(PB2_PROTOS))

PB3_OUTPUT_DIR=$(shell pwd)
PB3_PROTOS := $(wildcard *.proto)
PB3_SRCS   := $(patsubst %.proto, $(PB3_OUTPUT_DIR)/%.pb.cc, $(PB3_PROTOS)) $(patsubst %.proto, $(PB3_OUTPUT_DIR)/%_pb2.py, $(PB3_PROTOS))

#all : prepare $(PB2_SRCS) $(PB3_SRCS)
all : prepare $(PB3_SRCS) golang
	
t :
	@echo $(PB2_OUTPUT_DIR)
	@echo $(PB2_PROTOS)
	@echo $(PB2_SRCS)
	@echo $(PB3_OUTPUT_DIR)
	@echo $(PB3_PROTOS)
	@echo $(PB3_SRCS)

clean :
	@rm -rf $(PB2_SRCS) $(PB3_SRCS) *.py *.h *.cc *.o *.d pb2_output pb3_output 
	@rm -rf lcs lcs_common lcs_session_kv

golang : prepare
	$(PROTOC) --go_out=import_path=lcs_common:$(OUTPUT_DIR)/lcs_common lcs_common.proto
	$(PROTOC) --go_out=import_path=lcs,Mlcs_common.proto=goapp/lcspush/lcs/lcs_common:$(OUTPUT_DIR)/lcs lcs_extra.proto
	$(PROTOC) --go_out=import_path=lcs,Mlcs_common.proto=goapp/lcspush/lcs/lcs_common:$(OUTPUT_DIR)/lcs lcs_push.proto
	$(PROTOC) --go_out=import_path=lcs_session_kv,Mlcs_common.proto=goapp/lcspush/lcs/lcs_common:$(OUTPUT_DIR)/lcs_session_kv lcs_session_kv.proto
	$(PROTOC) --go_out=import_path=lcs,Mlcs_common.proto=goapp/lcspush/lcs/lcs_common:$(OUTPUT_DIR)/lcs lcs_session.proto
	$(PROTOC) --go_out=import_path=lcs,Mlcs_common.proto=goapp/lcspush/lcs/lcs_common:$(OUTPUT_DIR)/lcs lcs_msgcenter.proto


prepare:
	mkdir -p pb2_output
	mkdir -p pb3_output
	mkdir -p $(OUTPUT_DIR)/lcs
	mkdir -p $(OUTPUT_DIR)/lcs_common
	mkdir -p $(OUTPUT_DIR)/lcs_session_kv

#$(PB2_OUTPUT_DIR)/%.pb.cc : %.proto
#	protoc --cpp_out=$(PB2_OUTPUT_DIR) $<
#$(PB2_OUTPUT_DIR)/%_pb2.py : %.proto
#	protoc --python_out=$(PB2_OUTPUT_DIR) $<

$(PB3_OUTPUT_DIR)/%.pb.cc : %.proto
	/home/s/safe/bin/protoc3 --cpp_out=$(PB3_OUTPUT_DIR) $<
$(PB3_OUTPUT_DIR)/%_pb2.py : %.proto
	/home/s/safe/bin/protoc3 --python_out=$(PB3_OUTPUT_DIR) $<
