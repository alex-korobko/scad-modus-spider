include ../indTestMode
include ../indLfiles
include ../indOfiles
include ../indHfiles
include ../indIpath
include ../indLpath
include ../indVpath

BUILD_TYPE=RELEASE

#BUILD_TYPE=DEBUG

ifdef BUILD_TYPE
BUILD_RELEASE = -O3 -w8 -fomit-frame-pointer
BUILD_DEBUG = -O0 -w8 -g
BUILD=$(BUILD_$(BUILD_TYPE))
else
BUILD=-O0 -w8 -g
endif

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O
endif

CXX = qcc
CXXFLAGS = -Vgcc_ntox86 -fexceptions -Bdynamic $(BUILD) $(MYLIBS) $(MYIPATH) $(MYLPATH)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o modbus_srv

clean:
	rm -f *.o *.map modbus_srv

$(OBJECTS) : $(HEADERS)
