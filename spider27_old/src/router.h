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
#include <snmp/asn1.h>
#include <snmp/snmp_api.h>
#include <snmp/snmp.h>
#include <snmp/snmp_client.h>
#include <snmp/mib.h>
#include <string.h>

enum {addRoute, delRoute};

class Net
{
	friend class Router;
protected:
	in_addr_t	ip;
	int			broken;
	Net*		next;
	Net*		prev;
public:
	Net();
	virtual ~Net();
};

class Router
{
protected:
	Net*			routeTable;
	pthread_t		routerTID;
	int				tableSize;
	oid 				reqOid[MAX_OID_LEN];
	int 				reqOidLen;
	Net*			leftLine;
	Net	*			rightLine;
	Net*			leftLineTail;
	Net*			rightLineTail;
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

int SendRouteMsg(in_addr* host, in_addr* gate);

#endif
