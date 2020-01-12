# Originally sourced from
# https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

# Easier debug switch?
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g
LDFLAGS=
LDLIBS=
MKDIR_P=mkdir -p

SRCS=
OBJS=

PROTO_SRCS=orihime.proto

BUILD_DIRECTORY=build
PROTO_DIRECTORY=build/proto

SERVER_CLIENT_LDFLAGS=-L/usr/local/lib `pkg-config --libs protobuf grpc++` -pthread -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl
SERVER_LDFLAGS=-lpthread -lmysqlcppconn -lmysqlclient

all: $(BUILD_DIRECTORY)/orihime-server $(BUILD_DIRECTORY)/orihime-client

directories: $(BUILD_DIRECTORY) $(PROTO_DIRECTORY)

$(BUILD_DIRECTORY): 
	$(MKDIR_P) $(BUILD_DIRECTORY)

$(PROTO_DIRECTORY): 
	$(MKDIR_P) $(PROTO_DIRECTORY)


# The rules here need to be cleaned up, `make -j8 all` gives issues
# with there not being rules for the output files of orihime.grpc.pb.o
# (which makes sense)
#
# Also, there are rebuilds happening that are unnecessary and
# incorrect
PROTO_PB_C_SRC_NAMES=$(patsubst %.proto,$(PROTO_DIRECTORY)/%.pb,$(PROTO_SRCS))
PROTO_GRPC_C_SRC_NAMES=$(patsubst %.proto,$(PROTO_DIRECTORY)/%.grpc.pb,$(PROTO_SRCS))
PROTO_C_SRCS=$(patsubst %,%.cc,$(PROTO_PB_C_SRC_NAMES) $(PROTO_GRPC_C_SRC_NAMES)) $(patsubst %,%.h,$(PROTO_PB_C_SRC_NAMES) $(PROTO_GRPC_C_SRC_NAMES))
PROTO_C_OBJS=$(patsubst %,%.o,$(PROTO_PB_C_SRC_NAMES) $(PROTO_GRPC_C_SRC_NAMES))

proto: $(PROTO_C_SRCS) $(PROTO_DIRECTORY)
	find $(PROTO_DIRECTORY)

$(PROTO_DIRECTORY)/%.pb.h $(PROTO_DIRECTORY)/%.grpc.cc \
$(PROTO_DIRECTORY)/%.grpc.h $(PROTO_DIRECTORY)/%.pb.cc: %.proto $(PROTO_DIRECTORY)
	protoc --grpc_out=$(PROTO_DIRECTORY) --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<
	protoc --cpp_out=$(PROTO_DIRECTORY) $<

$(PROTO_DIRECTORY)/%.o: $(PROTO_DIRECTORY)/%.cc
	$(CXX) $(CPPFLAGS) `pkg-config --cflags protobuf grpc` -c -o $@ $<




SERVER_SRCS=$(shell find server -name \*.cc -printf "%P\n" | grep -v orihime-server.cc | tr '\n' ' ')
SERVER_OBJS=$(patsubst %.cc,$(BUILD_DIRECTORY)/%.o,$(SERVER_SRCS))

$(BUILD_DIRECTORY)/orihime-server.o: server/orihime-server.cc
	$(CXX) $(CPPFLAGS)  -I$(PROTO_DIRECTORY) `pkg-config --cflags protobuf grpc` -c -o $(BUILD_DIRECTORY)/orihime-server.o server/orihime-server.cc

$(BUILD_DIRECTORY)/%.o: server/%.cc
	$(CXX) $(CPPFLAGS) -I$(PROTO_DIRECTORY) -std=c++17 -c -o $@ $<

$(BUILD_DIRECTORY)/orihime-server: \
		server/orihime-server.cc \
		$(SERVER_OBJS) \
		$(BUILD_DIRECTORY)/orihime-server.o \
		$(PROTO_C_OBJS)
	$(CXX) \
		$(PROTO_C_OBJS) \
		$(SERVER_OBJS) \
		$(BUILD_DIRECTORY)/orihime-server.o \
		$(SERVER_CLIENT_LDFLAGS) $(SERVER_LDFLAGS) \
		-o $(BUILD_DIRECTORY)/orihime-server





$(BUILD_DIRECTORY)/orihime-client.o: client/orihime-client.cc
	$(CXX) $(CPPFLAGS)  -I$(PROTO_DIRECTORY) `pkg-config --cflags protobuf grpc` -c -o $(BUILD_DIRECTORY)/orihime-client.o client/orihime-client.cc

$(BUILD_DIRECTORY)/orihime-client: client/orihime-client.cc $(BUILD_DIRECTORY)/orihime-client.o $(PROTO_C_OBJS)
	$(CXX) $(CPPFLAGS) -I$(PROTO_DIRECTORY) `pkg-config --cflags protobuf grpc` -c -o $(BUILD_DIRECTORY)/orihime-client.o client/orihime-client.cc
	$(CXX) $(SERVER_CLIENT_LDFLAGS) \
		$(PROTO_DIRECTORY)/orihime.pb.o $(PROTO_DIRECTORY)/orihime.grpc.pb.o $(BUILD_DIRECTORY)/orihime-client.o \
		-o $(BUILD_DIRECTORY)/orihime-client

.PHONY: directories proto all
