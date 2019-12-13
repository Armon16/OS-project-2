
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++11 -Wall


all: main

headers: manager.h
	
main: headers main.cpp
	${CXX} main.cpp -o main

clean:
	rm -f main
