CXX = g++
CXXFLAGS = -O3 -std=c++17
OPENCV = `pkg-config --cflags --libs opencv`

.PHONY: all main clean

all: main

main: image_channels.o median_filter.o main.o
	$(CXX) $(CXXFLAGS) $(OPENCV) image_channels.o median_filter.o main.o -o Main

image_channels.o median_filter.o main.o:
	$(CXX) $(CXXFLAGS) -c image_channels.cpp
	$(CXX) $(CXXFLAGS) -c median_filter.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o Main	
