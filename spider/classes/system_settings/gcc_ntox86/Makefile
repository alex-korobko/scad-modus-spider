include ../indTestMode
include ../indLfiles
include ../indOfiles
include ../indHfiles

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -g
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g -w8
endif

ifndef OPTIM
OPTIM = -O
endif

CXX = qcc
CXXFLAGS = -Vgcc_ntox86 -fexceptions -Bdynamic $(DEBUG) $(MYLIB) -I. -I /usr/local/include/ -L /usr/local/lib/ -L /usr/lib/gcc-lib/ntox86/2.95.3qnx-nto/exceptions -L /x86/lib

VPATH =../ :\
 ../system_settings_classes :\
 ../unit_tests/system_settings_classes_unit_tests :\
 ../unit_tests

all: $(MYOBJ) 
	$(CXX) $(MYOBJ) $(CXXFLAGS) -M -o system_settings_test

clean:
	rm -f *.o *.map system_settings_test

$(MYOBJ) : $(MYHDR)
