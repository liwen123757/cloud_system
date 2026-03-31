TARGET=server_app
SRC=/home/ubuntu/cloud_system/cloude_system/server/service.cc
OBJ=$(SRC:.cc=.o)
CXX=g++
CXXFLAGS=-g -std=c++11 -I/home/ubuntu/cloud_system/cloude_system/server
LDFLAGS=-L/home/ubuntu/cloud_system/cloud/lib -lpthread -lstdc++fs -ljsoncpp -lbundle

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)