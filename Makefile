CXX=g++
CPPFLAGS=-std=c++11
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

sim: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o sim $(OBJECTS)

clean:
	rm -f $(OBJECTS)