include ../indTestMode
include ../indVpath
include ../indIpath
include ../indLpath
include ../indLfiles
include ../indOfiles
include ../indHfiles

DBG=RELEASE

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -g
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g
endif

CXX = qcc
CXXFLAGS = -Vgcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(LIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJS) 
	$(CXX) $(OBJS)  $(CXXFLAGS) -M -o testcomport

clean:
	rm -f *.o *.map testcomport

$(OBJS) : $(HDRS)

