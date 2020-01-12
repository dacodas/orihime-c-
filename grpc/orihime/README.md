# TODO

- Figure out gRPC reporting and whatnot
- Report mySQL errors back to the client if possible. Is this helpful?
  For me, sure, for a typical user? (Prolly shouldn't worry about
  "typical users" for a while)

# Test compilations

```
g++ -g `pkg-config --cflags protobuf grpc` -Ibuild/proto/ -c -o build/orihime-client.o client/orihime-client.cc -fdiagnostics-color=always & 
g++ -g `pkg-config --cflags protobuf grpc` -Ibuild/proto/ -c -o build/add.o client/add.cc -fdiagnostics-color=always & 
g++ -g `pkg-config --cflags protobuf grpc` -Ibuild/proto/ -c -o build/util.o client/util.cc -fdiagnostics-color=always & 
g++ -g `pkg-config --cflags protobuf grpc` -Ibuild/proto/ -c -o build/grpc.o client/grpc.cc -fdiagnostics-color=always & 

g++ -L/usr/local/lib `pkg-config --libs protobuf grpc++` -pthread -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl \
	build/proto/orihime.pb.o \
    build/proto/orihime.grpc.pb.o \
    build/orihime-client.o \
    build/add.o \
    build/grpc.o \
    build/util.o \
	-o build/orihime-client
```
