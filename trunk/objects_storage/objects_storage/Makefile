include indPlatform

.POHNY: all clean test untest linter

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

test: 
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) test) ; done

untest: 
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) untest) ; done

linter:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) linter) ; done

mock:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) mock) ; done
