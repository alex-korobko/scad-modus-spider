include indPlatform

.POHNY: all clean arch test untest

all:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

arch: clean
	rm -f lines_switcher.tgz
	tar -czf * lines_switcher.tgz

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > indTestMode

untest: 
	echo  '//Don`t edit that file! It was created automatically' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > indTestMode
