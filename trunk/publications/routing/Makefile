DIRS=router_pf_route router_ioctl

.POHNY: all clean arch

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) clean) ; done
	rm -f ../routing.tgz *.tgz

arch: clean
	tar -czf ../routing.tgz *