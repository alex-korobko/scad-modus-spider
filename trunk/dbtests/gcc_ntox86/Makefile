include ../indTestMode
include ../indDbobjects
include ../indTests

include ../indLibs

include ../indLpath
include ../indIpath
include ../indVpath

include ../indOfiles
include ../indHfiles

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O
endif

CXX = qcc
CXXFLAGS = -Vgcc_ntox86 -w8 -fexceptions -Bdynamic $(DEBUG) $(IPATH) $(LPATH) $(LIBS)

all: $(OBJS) 
	$(CXX) $(OBJS)  $(CXXFLAGS) -M -o dbtests

clean:
	rm -f *.o *.map dbtests

$(OBJS) : $(HDRS)

