EXENAME = main
OBJS = main.o png/HSLAPixel.o png/PNG.o png/lodepng/lodepng.o

CXX = g++
CXXFLAGS = $(CS225) -std=c++14 -c -g -O0 -Wall -pedantic
# CXX = clang++
# CXXFLAGS = $(CS225) -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LD = g++
LDFLAGS = -std=c++14 -pthread -lm

all: $(EXENAME)

$(EXENAME) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME)

main.o : main.cpp 
	$(CXX) $(CXXFLAGS) main.cpp

HSLAPixel.o : png/HSLAPixel.cpp
	$(CXX) $(CXXFLAGS) png/HSLAPixel.cpp

PNG.o : png/PNG.cpp
	$(CXX) $(CXXFLAGS) png/PNG.cpp

lodepng.o : png/lodepng/lodepng.cpp
	$(CXX) $(CXXFLAGS) png/lodepng/lodepng.cpp

clean :
	-rm -f *.o $(EXENAME)