#
# Application Makefile   
#



ifdef DBG
DBG_RELEASE = -O3 -fomit-frame-pointer
DBG_DEBUG = -O -gdwarf-2
DEBUG=$(DBG_$(DBG))
endif

ifndef DEBUG
DEBUG = -g
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


LIBPQDIR= ../libpq
PLATFORM=gcc_ntox86
PHABOPTS= 

CC  = qcc
CXX = qcc
LD  = qcc

CFLAGS  = -V$(PLATFORM) -w5 $(OPTIM) $(DEBUG) $(PHABOPTS) -I.
CXXFLAGS = $(CFLAGS)
LDFLAGS = -V$(PLATFORM) -Bstatic $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)
SDFLAGS = -V$(PLATFORM)          $(DEBUG) -l phexlib $(ABLIB) $(PHABOPTS)


CFLAGS+= -I$(LIBPQDIR) -I/usr/include/pgsql -I. -I.. -I../misc
LDFLAGS+= -L$(LIBPQDIR) -lpq
SDFLAGS+= -L$(LIBPQDIR) -lpq
VPATH = ..

#
# Application Program
#

shr:	$(ABOBJ) $(MYOBJ)
		$(LD) $(ABOBJ) $(MYOBJ) $(SDFLAGS) -M -o converter
		usemsg converter ../Usemsg
		phabbind converter $(ABMOD)

app:	$(ABOBJ) $(MYOBJ)
		$(LD) $(ABOBJ) $(MYOBJ) $(LDFLAGS) -M -o converter
		usemsg converter ../Usemsg
		phabbind converter $(ABMOD)

clean:
		rm -f *.o *.err *.map converter

install:
		-cp -vnf converter /usr/photon/bin

#
# Headers
#

$(ABOBJ) $(MYOBJ):	$(MYHDR)

#
# Prototype Generation
#

proto:
		approto -p $(ABSRC) $(MYSRC) > proto.h
