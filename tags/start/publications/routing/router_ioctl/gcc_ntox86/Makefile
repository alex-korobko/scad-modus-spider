include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath
include ../indLpath
include ../indVpath

DEBUG_MODE=yes

ifdef DEBUG_MODE
DEBUG = -g -O0 -w8
endif

CXX = qcc
CXXFLAGS = -V gcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o router_ioctl

clean:
	rm -f *.o *.map router_ioctl

$(OBJECTS) : $(HEADERS)

