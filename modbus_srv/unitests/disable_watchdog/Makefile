CC=qcc
CFLAGS=-g -O -w8
LDFLAGS=

OBJS = disable_watchdog.o 
SRCS = disable_watchdog.cpp 
HDRS =

all: 
	$(CC) $(SRCS) -o disable_watchdog $(CFLAGS) $(LDFLAGS) -lsocket
    
clean:
	rm -f disable_watchdog *.o *.err
