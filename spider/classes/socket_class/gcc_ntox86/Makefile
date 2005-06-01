include ../indTestMode
include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath


ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -g
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O
endif

CXX = qcc
CXXFLAGS = -Vgcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIBS) $(MYINCLUDES) -L /usr/local/lib/ -L /usr/lib/gcc-lib/ntox86/2.95.2/exceptions -L /usr/ntox86/lib/exceptions

VPATH =../socket_classes :\
 ../unit_tests :\
 ../unit_tests/socket_classes_unit_tests :\
 ../ :\
 ../system_settings_classes

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o sockets

clean:
	rm -f *.o *.map sockets

$(OBJECTS) : $(HEADERS)
