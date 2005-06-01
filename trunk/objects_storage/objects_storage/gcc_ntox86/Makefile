include ../indTestMode
include ../indDbwrapper
include ../indVpath
include ../indIpath
include ../indLpath

include ../indLfiles
include ../indOfiles
include ../indHfiles

.POHNY: all clean test untest linter

DEBUG=-w8
OPTIM=-O3

ifndef DEBUG
DEBUG = -g -w8
endif

ifndef OPTIM
OPTIM = -O0
endif

CXX = qcc
CC = $(CXX)
CXXFLAGS = \
   -V gcc_ntox86 \
   -fexceptions \
   -Bstatic \
    $(DEBUG) \
    $(OPTIM) \
    $(MYIPATH) \
    $(MYLPATH) \
    $(MYLIBS)

CFLAGS=$(CXXFLAGS)

all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o depository

clean:
	rm -f *.o *.map depository

$(OBJECTS) : $(HEADERS)

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > ../test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > ../indTestMode

untest: 
	echo  '//Don`t edit that file! It was created automatically' > ../test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > ../indTestMode

linter:
	echo  '//Don`t edit that file! It was created automatically\n#define _LINTER_DBWRAPPER_' > ../dbwrapper_definition.h
	echo  '#Don`t edit that file! It was created automatically\nLINTER_DBWRAPPER = 1\n' > ../indDbwrapper

extreamedb:
	echo  '//Don`t edit that file! It was created automatically\n#define _EXTREAME_DB_DBWRAPPER_' > ../dbwrapper_definition.h
	echo  '#Don`t edit that file! It was created automatically\nEXTREAME_DB_DBWRAPPER = 1\n' > ../indDbwrapper

mock:
	echo  '//Don`t edit that file! It was created automatically\n#define _MOCK_DBWRAPPER_' > ../dbwrapper_definition.h
	echo  '#Don`t edit that file! It was created automatically\nMOCK_DBWRAPPER = 1\n' > ../indDbwrapper
