ALLDIRS=objects_storage requests_generator
#DATECMD=date "+%C%y-%m-%d"
#DATEVALUE=$(DATECMD)

.POHNY: all arh clean

all:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) all) ; done

arh: clean
	rm -f objects_storage*.tgz	
#	$(DATECMD)
#	tar -czf objects_storage$(DATECMD).tgz *
	tar -czf objects_storage.tgz *

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done
