include ../indTestMode
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
CXXFLAGS = -V gcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIBS) -I. -I /usr/local/include/ -L /usr/local/lib/ -L /usr/lib/gcc-lib/ntox86/2.95.3/exceptions -L /usr/ntox86/lib/exceptions

VPATH = ../metro_devices_classes\
 :../unit_tests\
 :../unit_tests/metro_devices_classes_unit_tests\
 :../\
 :../system_settings_classes\
 :../com_port_class\
:../containers_classes

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o station_emulator

clean:
	rm -f *.o *.map station_emulator



$(OBJECTS) : $(HEADERS)

