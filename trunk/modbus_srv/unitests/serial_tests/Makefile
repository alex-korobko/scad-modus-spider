CC=qcc
CFLAGS=-O0 -g
LDFLAGS=
SRCROOT=../../

OBJS = modbus.o system.o lbuffer.o routeinfo.o test.o
SRCS = $(SRCROOT)modbus.cpp  $(SRCROOT)system.cpp $(SRCROOT)lbuffer.cpp $(SRCROOT)routeinfo.cpp test.cpp
HDRS = modbus.h system.h lbuffer.h global.h types.h routeinfo.h test.h

all: 
	$(CC) $(SRCS) -o test $(CFLAGS) $(LDFLAGS) -lsocket
    
clean:
	rm -f test *.o *.err
