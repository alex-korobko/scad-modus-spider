CC=qcc
CFLAGS=-O -w8
LDFLAGS=

OBJS = modbus.o modbus_srv.o system.o lbuffer.o 
SRCS = modbus.cpp modbus_srv.cpp system.cpp lbuffer.cpp 
HDRS = modbus.h modbus_srv.h system.h lbuffer.h global.h types.h watchdog.h

all: 
	$(CC) $(SRCS) -o modbus_srv $(CFLAGS) $(LDFLAGS) -lsocket
    
clean:
	rm -f modbus_srv *.o *.err
