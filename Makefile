all:
	g++ -std=c++0x $(wildcard *.cpp) $(wildcard client/*.cpp) -o searchClient
	
test:
	g++ -std=c++0x $(wildcard *.cpp) $(wildcard tests/*.cpp) -o tests