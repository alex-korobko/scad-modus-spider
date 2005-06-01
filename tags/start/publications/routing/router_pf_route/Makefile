include indPlatform

.POHNY: all clean arch

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done
	rm -f ../router_pf_route.tgz

arch: clean
	tar -czf ../router_pf_route.tgz ../router_pf_route ../support_classes