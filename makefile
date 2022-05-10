CXX=g++
CFLAGS=-std=c++11 -O2 -Wall -g
CXXFLAGS=-std=c++11 -O2 -Wall -g

TARGET:=myserver
SOURCE:=$(wildcard ../*.cpp)
OBJS=./buffer.cpp ./request.cpp ./response.cpp ./connect.cpp ./threadpool.cpp\
     ./timer.cpp ./epoller.cpp ./webserver.cpp ./main.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) -o ./bin/$(TARGET) -pthread

