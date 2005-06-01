include indPlatform

.POHNY: all clean arch

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

arch: clean
	tar -czf ../router_ioctl.tgz ../router_ioctl ../support_classes