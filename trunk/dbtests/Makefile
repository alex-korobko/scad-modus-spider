include indPlatform

.POHNY: all clean test untest

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > src/test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > indTestMode

untest: 
	echo  '//Don`t edit that file! It was created automatically' > src/test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > indTestMode
