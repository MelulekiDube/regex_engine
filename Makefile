#Make file adapted from https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/
#


CC := gcc 
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := bin/runner
TTARGET := bin/tester
WFIND := C:\cygwin64\bin\find

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall
LIB := -lcunit
INC := -I include

compile: $(TARGET)


$(TARGET): $(OBJECTS)
	@echo " Linking...";
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET) $(LINKLIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) clean
	@mkdir -p $(BUILDDIR);
	@echo "compiling the source files"
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

run:
	./$(TARGET)

tester:
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) test/*.c $(SOURCES) $(INC) $(LIB) -o $(TTARGET)

testrunner:
	./$(TTARGET)

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(TARGET) $(BINDIR) $(TTARGET)

.PHONY: clean