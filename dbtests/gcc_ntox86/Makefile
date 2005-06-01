include ../indTestMode
include ../indDbobjects
include ../indTests
include ../indVpath
include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indSfiles

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
CXXFLAGS = -Vgcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(LIBS) -I. -I /usr/local/include/ -L /usr/local/lib/ -L /usr/lib/gcc-lib/ntox86/2.95.2/exceptions -L /usr/ntox86/lib/exceptions

all: $(OBJS) 
	$(CXX) $(OBJS)  $(CXXFLAGS) -M -o dbtests

clean:
	rm -f *.o *.map dbtests

$(OBJS) : $(HDRS) $(SRCS)

