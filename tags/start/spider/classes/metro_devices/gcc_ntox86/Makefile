include ../indTestMode
include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath
include ../indLpath
include ../indVpath

ifndef DEBUG
DEBUG = -g -w8
endif

ifndef OPTIM
OPTIM = -O0
endif

CXX = qcc
CXXFLAGS = -V gcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o metro_dev_test

clean:
	rm -f *.o *.map metro_dev_test


$(OBJECTS) : $(HEADERS)

