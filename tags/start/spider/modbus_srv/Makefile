include indPlatform

.POHNY: all clean test untest

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING\n' > indTestMode

untest: 
	echo  '//Don`t edit that file! It was created automatically' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > indTestMode
