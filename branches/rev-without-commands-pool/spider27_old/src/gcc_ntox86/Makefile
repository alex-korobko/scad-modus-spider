#
# Application Makefile   
#

ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O0 -g
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g -O0
endif

ifndef OPTIM
OPTIM = -O
endif

include	../abOfiles
include ../indOfiles

include	../abSfiles
include ../indSfiles

include	../abHfiles
include ../indHfiles

include	../abWfiles
include	../abLfiles

PLATFORM=gcc_ntox86
PHABOPTS= 

CC  = qcc
CXX = qcc
LD  = qcc

CFLAGS  = -V$(PLATFORM) -w5 $(OPTIM) $(DEBUG) $(PHABOPTS) -I.
CXXFLAGS = $(CFLAGS)
LDFLAGS = -V$(PLATFORM) -Bstatic $(DEBUG) -l phexlib -l socket $(ABLIB) $(PHABOPTS)
SDFLAGS = -V$(PLATFORM) -Bdynamic $(DEBUG) -l phexlib -l socket -l asound $(ABLIB) $(PHABOPTS)

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
