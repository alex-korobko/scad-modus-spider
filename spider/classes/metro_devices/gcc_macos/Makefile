include ../indTestMode
include ../indLfiles
include ../indOfiles_macos
include ../indHfiles_macos
include ../indLfiles_macos
include ../indLpath_macos
include ../indIpath
include ../indVpath

ifndef DEBUG
DEBUG = -g -Wall
endif

ifndef OPTIM
OPTIM = -O0
endif

# Use standard gcc/g++ compiler for macOS
CXX = g++
CXXFLAGS = -std=c++11 -fexceptions -pthread $(DEBUG) $(OPTIM) $(MYIPATH) -I.

# macOS-specific libraries (replacing QNX-specific ones)
MYLIBS = -lm -lpthread

# Note: cppunit is excluded since unit tests are not included in macOS build
# ifdef UNIT_TESTING
# MYLIBS += -lcppunit
# endif

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(MYLPATH) $(MYLIBS) -o metro_dev_test

clean:
	rm -f *.o *.map metro_dev_test

$(OBJECTS) : $(HEADERS)
