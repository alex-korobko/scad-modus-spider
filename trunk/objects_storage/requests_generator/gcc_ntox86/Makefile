include ../indTestMode

include ../indVpath
include ../indIpath
include ../indLpath

include ../indLfiles
include ../indOfiles
include ../indHfiles


ifndef DEBUG
DEBUG = -g -w8
endif

ifndef OPTIM
OPTIM = -O0
endif


CXX = qcc
CXXFLAGS = \
   -V gcc_ntox86 \
   -fexceptions \
   -Bstatic \
    $(OPTIM) \
    $(DEBUG) \
    $(MYIPATH) \
    $(MYLPATH) \
    $(MYLIBS)


all: $(OBJECTS) 
	$(CXX) $(OPTIM) $(OBJECTS) $(CXXFLAGS) -M -o requests_generator

clean:
	rm -f *.o *.map requests_generator


$(OBJECTS) : $(HEADERS)

