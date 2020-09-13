CXX=g++ -std=c++17

SRC=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SRC))

TARGET=csg
INCDIRS=-I../../src/ -I../../ -I./

LDFLAGS=-Wl, -framework GLUT -framework OpenGL -lGLEW -lglfw

# define a newline function to run each test as a separate command
define \n


endef


all: build

%.o: %.cpp
	$(CXX) $(INCDIRS) -c -o $@ $<

build: $(OBJS)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)


clean:
	rm -rf $(OBJS)
	rm -r $(TARGET)

.PHONY: clean default
