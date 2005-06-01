include ../../indPlatform

.POHNY: all clean test untest linter extremedb

all:
	for i in $(ALLDIRS);  do ( cd ../../ && $(MAKE) all) ; done
 
clean:
	for i in $(ALLDIRS) ;do ( cd ../../ && $(MAKE) clean) ; done

test: 
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) test) ; done

untest: 
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) untest) ; done

linter:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) linter) ; done

extreamedb:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) extreamedb) ; done

mock:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) mock) ; done
