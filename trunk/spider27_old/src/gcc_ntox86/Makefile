#
# Application Makefile   
#

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -g
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O0
endif

include	../abOfiles
include ../indOfiles

include	../abSfiles
include ../indSfiles

include	../abHfiles
include ../indHfiles

include	../abWfiles
include	../abLfiles

include ../indLfiles

PLATFORM=gcc_ntox86
PHABOPTS= 

CC  = qcc
CXX = qcc
LD  = qcc

CFLAGS  = -V$(PLATFORM) -w8 $(OPTIM) $(DEBUG) $(PHABOPTS) -I. -I /usr/local/include/ -L /usr/local/lib/
CXXFLAGS = $(CFLAGS)
LDFLAGS = -vv -V$(PLATFORM) -Bstatic $(DEBUG) $(MYLIBS) $(ABLIB) $(PHABOPTS)
SDFLAGS = -vv -V$(PLATFORM) -Bdynamic $(DEBUG) $(MYLIBS) $(ABLIB) $(PHABOPTS) 

VPATH = ..

#
# Application Program
#

shr:	$(ABOBJ) $(MYOBJ)
		$(LD) $(ABOBJ) $(MYOBJ) $(SDFLAGS) -M -o spider
		usemsg spider ../Usemsg
		phabbind spider $(ABMOD)

app:	$(ABOBJ) $(MYOBJ)
		$(LD) $(ABOBJ) $(MYOBJ) $(LDFLAGS) -M -o spider
		usemsg spider ../Usemsg
		phabbind spider $(ABMOD)

clean:
		rm -f *.o *.err *.map spider

install:
		-cp -vnf spider /usr/photon/bin

#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
