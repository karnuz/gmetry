CXX=g++ -std=c++17

OBJDIR=bin
SRCDIR=src
TESTDIR=test
TEST_OBJDIR=bin/test
LIBNAME=karnuz
EXAMPLEDIR=examples
EXAMPLEOBJDIR=examples/bin
LIB=$(OBJDIR)/lib$(LIBNAME).so

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

TEST_SRC=$(wildcard $(TESTDIR)/*.cpp)
TEST_OBJS=$(patsubst $(TESTDIR)/%.cpp, $(TEST_OBJDIR)/%.o, $(TEST_SRC))
TEST_BINS=$(patsubst %.o, %, $(TEST_OBJS))

EXAMPLES_SRC=$(wildcard $(EXAMPLEDIR)/*.cpp)
EXAMPLES_OBJS=$(patsubst $(EXAMPLEDIR)/%.cpp, $(EXAMPLEOBJDIR)/%.o, $(EXAMPLES_SRC))
EXAMPLES_BINS=$(patsubst %.o, %, $(EXAMPLES_OBJS))

CFLAGS=-Isrc/ -framework GLUT -framework OpenGL
#EXAMPLES_CFLAGS=-Isrc/ -framework GLUT -framework OpenGL

TEST_LDFLAGS=-Wl,
EXAMPLES_LDFLAGS=-Wl, -framework GLUT -framework OpenGL
# define a newline function to run each test as a separate command
define \n


endef



$(OBJDIR):
	mkdir $@

$(EXAMPLEOBJDIR):
	mkdir $@

tests: $(TEST_OBJDIR) $(TEST_BINS)

$(TEST_BINS): $(TEST_OBJS)

$(TEST_OBJDIR): $(OBJDIR)
	mkdir $@

$(TEST_OBJDIR)/%: $(TEST_OBJDIR)/%.o
	$(CXX) -g -o $@ $< $(TEST_LDFLAGS)

$(TEST_OBJDIR)/%.o: $(TESTDIR)/%.cpp $(OBJDIR)
	$(CXX) -D_DEBUG -g -O0 -I$(SRCDIR) -c -o $@ $<

check: tests
	@echo "Running tests..."
	$(foreach TEST, $(TEST_BINS), @./$(TEST) 2>&1 1>/dev/null && printf "$(TEST) \t Pass\n" || printf "$(TEST) \t Fail\n";${\n})

egs: $(EXAMPLEOBJDIR) $(EXAMPLES_BINS)

$(EXAMPLES_BINS): $(EXAMPLES_OBJS)

$(EXAMPLEOBJDIR)/%: $(EXAMPLEOBJDIR)/%.o
	$(CXX) -g -o $@ $< $(EXAMPLES_LDFLAGS)

$(EXAMPLEOBJDIR)/%.o: $(EXAMPLEDIR)/%.cpp
	$(CXX) -D_DEBUG -g -O0 -I$(SRCDIR) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)
	rm -r $(EXAMPLEOBJDIR)

.PHONY: clean default tests check
