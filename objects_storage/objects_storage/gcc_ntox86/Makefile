include ../indTestMode
include ../indDbwrapper
include ../indVpath
include ../indIpath
include ../indLpath

include ../indLfiles
include ../indOfiles
include ../indHfiles

.POHNY: all clean test untest linter

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
    $(DEBUG) \
    $(MYIPATH) \
    $(MYLPATH) \
    $(MYLIBS)


all: $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -M -o objects_storage

clean:
	rm -f *.o *.map objects_storage

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

mock:
	echo  '//Don`t edit that file! It was created automatically\n#define _MOCK_DBWRAPPER_' > ../dbwrapper_definition.h
	echo  '#Don`t edit that file! It was created automatically\nMOCK_DBWRAPPER = 1\n' > ../indDbwrapper
