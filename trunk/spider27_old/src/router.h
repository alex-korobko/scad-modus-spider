#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define SKIP_TEST_COUNT 4

enum {addRoute, delRoute};

class Net
{
	friend class Router;
protected:
	in_addr_t	ip;
	in_addr_t	curr_gateway_ip;
	int			broken;
	int skip_count;
	Net*		next;
	Net*		prev;
public:
	Net();
	Net* first_Net();
	Net* next_Net();
	virtual ~Net();
};

class Router
{
protected:
	Net*			routeTable;
	pthread_t		routerTID;
	int				tableSize;
	Net*			leftLine;
	Net*			rightLine;
	in_addr			leftGate, rightGate;
public:
	Router();
	virtual ~Router();
	void AddLeftLeafGate(char* gate);
	void AddRightLeafGate(char* gate);
	int CreateTable(int size);
	int LoadLeft(const char* filename);
	int LoadRight(const char* filename);
	int Start();
	void Loop();
	friend void* Routing(void* arg);
};

int ConnectToServer(in_addr_t addr, int port);
int Receive(int sock, byte* buffer, int size);
#endif
