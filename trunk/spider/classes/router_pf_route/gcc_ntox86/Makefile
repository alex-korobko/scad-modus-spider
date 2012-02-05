include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath
include ../indLpath
include ../indVpath

DEBUG_MODE=yes

ifndef DEBUG_MODE
DEBUG = -g -w8
endif

VPATH=../:../support_classes:../router_classes

CXX = qcc
CXXFLAGS = -V gcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o router_pf_route

clean:
	rm -f *.o *.map router_pf_route

$(OBJECTS) : $(HEADERS)

