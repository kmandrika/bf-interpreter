CXX = g++
CXXFLAGS = -Wall

bin/bf : src/bf.cxx include/*.h
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -Iinclude/ src/bf.cxx -o bin/bf
	ln -s -f bin/bf bf
