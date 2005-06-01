include abplatform

.POHNY: shr app proto install test untest

shr:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) shr) ; done

app:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) app) ; done

proto:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) proto) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

install:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) install) ; done

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__\n#define __DEBUG__' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > indTestMode

untest: 
	echo  '//Don`t edit that file! It was created automatically' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > indTestMode
