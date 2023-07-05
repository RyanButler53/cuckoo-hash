CXX = clang++
CXXFLAGS = -std=c++2a -g -Wall -pedantic
TARGET = cuckoo-test

all: $(TARGET)

cuckoo-test: cuckoo-test.o
	$(CXX) -o cuckoo-test cuckoo-test.o

cuckoo-test.o: cuckoo-test.cpp cuckoo-hash.hpp cuckoo-hash-private.hpp
	$(CXX) -c cuckoo-test.cpp $(CXXFLAGS)

clean: 
	rm -rf $(TARGET) *.o