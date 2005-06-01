include ../indTestMode
include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath
include ../indLpath
include ../indVpath

BUILD_TYPE=RELEASE

ifdef BUILD_TYPE
BUILD_RELEASE= -O1 -w8
BUILD_DEBUG= -g -O0 -w8
BUILD=$(BUILD_$(BUILD_TYPE))
else
BUILD=-O3 -w8
endif

ifndef OPTIM
OPTIM = -O0
endif

CXX = qcc
CXXFLAGS = -V gcc_ntox86 -fexceptions -Bdynamic $(BUILD) $(MYLIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o spider

clean:
	rm -f *.o *.map spider


$(OBJECTS) : $(HEADERS)

