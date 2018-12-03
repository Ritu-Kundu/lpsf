#
# TODO: 
#

CC := g++
# Define source-code directory 
SRCDIR := src

# Define build directory (for storing object files: will be removed on clean) 
BUILDDIR := build

# Define binary directory (for storing object files: will be removed on clean) 
BINDIR := bin


# Define executable name 
TARGET := bin/lpsf

# Define my lib name 
MYLIB := bin/lpsf.a
 
# Define libraries
SDSL_DIR := ./external/sdsl-lite/libsdsl
LIB := -L$(SDSL_DIR)/lib/ -lsdsl -ldivsufsort -ldivsufsort64 -Wl,-rpath=$(PWD)/$(SDSL_DIR)/lib

# Define include files (header <> file paths)
INC := -I include -I $(SDSL_DIR)/include/

 
# 
# No need to edit below this line other than to remvove sdsl
#
 
# Define source files
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

# Define object files
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS := -g -std=c++11 -D_USE_32 -msse3 -fopenmp -O3 -fomit-frame-pointer -funroll-loops -fPIC
LFLAGS= -O3 -DNDEBUG --shared

# Main entry point
#
all: $(MYLIB) $(TARGET)    

# For linking object file(s) to produce the library
#
$(MYLIB): $(OBJECTS)
	ar -rs $@ $(OBJECTS	)


# For linking object file(s) to produce the executable
#
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "============"
	@echo " Linking..."
	$(CC) $^ -o $(TARGET) $(LFLAG) $(LIB)
	@echo "============"

# For generating dependency
DEPS := $(OBJS:.o=.d)

-include $(DEPS)

# For compiling source file(s)
#
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "============"
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(INC) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

 
# For cleaning up the project
#
clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) $(DEPS)"; $(RM) -r $(BUILDDIR) $(TARGET) $(DEPS)
	

clean-all: 
	@echo " Cleaning all..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) $(DEPS)"; $(RM) -r $(BUILDDIR) $(TARGET) $(DEPS)
	


