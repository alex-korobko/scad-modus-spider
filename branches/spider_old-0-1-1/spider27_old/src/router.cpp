/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 28.10.2002
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include "global.h"
#include "router.h"
#include "system.h"

typedef struct {
	struct rt_msghdr		header;
	sockaddr					dest;
	sockaddr					netmask;
	sockaddr					gateway;
} RouteMsg;

int 	activeHost = 0, breakLine = 0;
int AsynchResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic);
int ChangeRoute(struct in_addr* dest, struct in_addr *gateway, int flags, int cmd);
char	lineStatusRID[] = ".1.3.6.1.2.1.2.2.1.8.2";

Net::Net()
{
	ip = 0;
	broken = 0;
	next = NULL;
	prev = NULL;
}

Net::~Net()
{
}

Router::Router()
{
	routeTable = NULL;
	leftLineTail = leftLine = new Net;
	rightLineTail = rightLine = new Net;

	init_mib();
	init_snmp();

	reqOidLen = sizeof(reqOid)/sizeof(reqOid[0]);
	if (!read_objid(lineStatusRID, reqOid, &reqOidLen))
	      SysMessage(ERROR_MSG, "can't read id %s", lineStatusRID);
}

void Router::AddLeftLeafGate(char* gate)
{
	leftGate.s_addr = inet_addr(gate);
}

void Router::AddRightLeafGate(char* gate)
{
	rightGate.s_addr = inet_addr(gate);
}

Router::~Router()
{
	if (pthread_cancel(routerTID) != EOK)
			SysMessage(ERROR_MSG, "can't terminate thread");

	if (routeTable)
		delete[] routeTable;
}

int Router::CreateTable(int size)
{
	routeTable = new Net[size];
	if (!routeTable)
		return 0;
	
	return 1;
}

int Router::LoadLeft(const char* filename)
{
	FILE*	fd;
	char	addr[16];
	Net* 	net = NULL;

	fd = fopen(filename, "rt");
	if (fd)
	{
		fscanf(fd, "%d\n", &tableSize);		
		if (tableSize)
		{
			for(int i=0; i<tableSize; i++)
			{
				fscanf(fd, "%s\n", addr);
				printf("Load net %s\n", addr);
				net = new Net;
				net->ip = inet_addr(addr);
				if (net)
				{
					leftLineTail->next = net;
					net->prev = leftLineTail;
					leftLineTail = net;
					if (i > 0)
					{
						in_addr netAddr;
						netAddr.s_addr = net->ip;
						ChangeRoute(&netAddr, &leftGate, 0, addRoute);
					}					
				}
				else
				{
					fclose(fd);
					return 0;
				}
			}
			net->broken = -1;

			fclose(fd);
			return 1;
		}
		fclose(fd);
		return 0;
	}
	
	return 0;
}

int Router::LoadRight(const char* filename)
{
	FILE*	fd;
	char	addr[16];
	Net* 	net = NULL;

	fd = fopen(filename, "rt");
	if (fd)
	{
		fscanf(fd, "%d\n", &tableSize);		
		if (tableSize)
		{
			for(int i=0; i<tableSize; i++)
			{
				fscanf(fd, "%s\n", addr);
				printf("Load net %s\n", addr);
				net = new Net;
				net->ip = inet_addr(addr);
				if (net)
				{
					rightLineTail->next = net;
					net->prev = rightLineTail;
					rightLineTail = net;
					if (i > 0)
					{
						in_addr netAddr;
						netAddr.s_addr = net->ip;
						ChangeRoute(&netAddr, &rightGate, 0, addRoute);
					}					
				}
				else
				{
					fclose(fd);
					return 0;
				}
			}
			net->broken = -1;

			fclose(fd);
			return 1;
		}
		fclose(fd);
		return 0;
	}
	
	return 0;
}

int Router::Start()
{
	if (pthread_create(&routerTID, NULL, &Routing, this) != EOK)
	{
		printf("Error: start thread\n");
		return 0;
	}
	
	return 1;         
}

void* Routing(void* arg)
{
	Router*		router;
	
	if (!arg)
		return 0;
		
	router = (Router*)arg;	
	
	while(1)
	{
		router->Loop();
		sleep(10);
	}
}

void Router::Loop()
{
	struct snmp_pdu 		*req;
	struct snmp_session 	sess;
	in_addr					addr;
//	char					name[16];
	u_char					community[] = "public";
	struct snmp_session 	*curSess;
	int	i;
	Net*					curNet;
	in_addr netAddr;
	int						reroute = 0;

  /* startup all hosts */

	curNet = leftLine->next;

//	printf("\nScan left\n");
	while(curNet)
	{
		breakLine = 0;
   		memset(&sess, 0, sizeof(sess));
	    	sess.version = SNMP_VERSION_1;
		addr.s_addr = (1 << 24) | curNet->ip;
		sess.peername = 	inet_ntoa(addr);
//		printf("IP: %s\n", sess.peername);
		sess.community = community;
		sess.community_len = 6;
		sess.callback = (int(*)())AsynchResponse;
		sess.callback_magic = curNet;
  		if (!(curSess = snmp_open(&sess)))
		{
  //   		printf("Error: snmp_open\n");
      		continue;
    		}
    		req = snmp_pdu_create(GET_REQ_MSG);
		snmp_add_null_var(req, reqOid, reqOidLen);
	  	if (snmp_send(curSess, req))
		{
      		activeHost = 1;
    		}
    		else
		{
  //   		printf("Error: snmp_send\n");
      		snmp_free_pdu(req);
    		}

		while (activeHost)
		{
  			int fds = 0, block = 0;
			fd_set fdset;
    			struct timeval timeout;

			FD_ZERO(&fdset);
    			timeout.tv_sec = 2;
    			timeout.tv_usec = 0;
    			snmp_select_info(&fds, &fdset, &timeout, &block);
    			fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
    			if (fds)
				snmp_read(&fdset);
    			else
				snmp_timeout();
		}
		snmp_close(curSess);
//		printf("Current broken %d\n", curNet->broken);
		if (breakLine)
		{
			if (curNet->broken == 1)
				curNet = NULL;
			else
			{
				curNet->broken = 1;
				curNet = curNet->next;
			}
			break;
		}
		else if (curNet->broken == 1)
		{
			curNet->broken = 0;
			Net* temp = curNet->next;
			while(temp)
			{
				netAddr.s_addr = temp->ip;
				ChangeRoute(&netAddr, &rightGate, 0, delRoute);
				ChangeRoute(&netAddr, &leftGate, 0, addRoute);
	//			printf("Restore net %s on gate ", inet_ntoa(netAddr));
		//		printf("%s\n", inet_ntoa(leftGate));
				temp = temp->next;
			}
		}
		curNet = curNet->next;
	}

	while(curNet)
	{
		curNet->broken = 0;
		netAddr.s_addr = curNet->ip;
		ChangeRoute(&netAddr, &leftGate, 0, delRoute);
		ChangeRoute(&netAddr, &rightGate, 0, addRoute);
	//	printf("Reroute net %s on gate ", inet_ntoa(netAddr));
	//	printf("%s\n", inet_ntoa(rightGate));	
		curNet = curNet->next;
	}

	curNet = rightLine->next;
//	printf("Scan right\n");
	while(curNet)
	{
		breakLine = 0;
   		memset(&sess, 0, sizeof(sess));
	    	sess.version = SNMP_VERSION_1;
		addr.s_addr = (2 << 24) | curNet->ip;
		sess.peername = 	inet_ntoa(addr);
	//	printf("IP: %s\n", sess.peername);
		sess.community = community;
		sess.community_len = 6;
		sess.callback = (int(*)())AsynchResponse;
		sess.callback_magic = curNet;
  		if (!(curSess = snmp_open(&sess)))
		{
 //     		printf("Error: snmp_open\n");
      		continue;
    		}
    		req = snmp_pdu_create(GET_REQ_MSG);
		snmp_add_null_var(req, reqOid, reqOidLen);
	  	if (snmp_send(curSess, req))
		{
//  		    	printf("Send\n");
      		activeHost = 1;
    		}
    		else
		{
    //  		printf("Error: snmp_send\n");
      		snmp_free_pdu(req);
    		}

		while (activeHost)
		{
  			int fds = 0, block = 0;
			fd_set fdset;
    			struct timeval timeout;

			FD_ZERO(&fdset);
    			timeout.tv_sec = 2;
    			timeout.tv_usec = 0;
    			snmp_select_info(&fds, &fdset, &timeout, &block);
    			fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
    			if (fds)
				snmp_read(&fdset);
    			else
				snmp_timeout();
		}
		snmp_close(curSess);
		if (breakLine)
		{
			if (curNet->broken == 1)
				curNet = NULL;
			else
			{
				curNet->broken = 1;
				curNet = curNet->next;
			}
			break;
		}
		else if (curNet->broken == 1)
		{
			curNet->broken = 0;
			Net* temp = curNet->next;
			while(temp)
			{
				netAddr.s_addr = temp->ip;
				ChangeRoute(&netAddr, &leftGate, 0, delRoute);
				ChangeRoute(&netAddr, &rightGate, 0, addRoute);
	//			printf("Restore net %s on %s gate\n", inet_ntoa(netAddr), inet_ntoa(rightGate));	
				temp = temp->next;
			}
		}
		curNet = curNet->next;
	}

	while(curNet)
	{
		curNet->broken = 0;
		netAddr.s_addr = curNet->ip;
		ChangeRoute(&netAddr, &rightGate, 0, delRoute);
		ChangeRoute(&netAddr, &leftGate, 0, addRoute);
//		printf("Reroute net %s on %s gate\n", inet_ntoa(netAddr), inet_ntoa(leftGate));
		curNet = curNet->next;
	}
//	printf("End scan\n");
}

int AsynchResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic)
{
	Net* net = (Net*)magic;
//	struct snmp_pdu *req;
	struct variable_list *vars;

	if (operation == RECEIVED_MESSAGE)
	{
  		if (pdu->errstat == SNMP_ERR_NOERROR)
		{
			for(vars = pdu->variables; vars; vars = vars->next_variable)
    				if (vars->type == ASN_INTEGER)
    				{
//		        		printf("Status %d\n", *(vars->val.integer));
					switch(*(vars->val.integer))
					{
						case 1:
							break;
						default:
							breakLine = 1;
							break;
					}
  		      	}
    		}
	}
  	else
	    	breakLine = 1;

 	activeHost = 0;
	
	return 1;
}

int ChangeRoute(struct in_addr* dest, struct in_addr *gateway, int flags, int cmd)
{
	int sock;
  	struct ortentry routeEntry;
  	struct sockaddr_in destAddr, gateAddr;

  	memset(&routeEntry, 0, sizeof(routeEntry));

  	memset(&destAddr, 0, sizeof(destAddr));
  	destAddr.sin_family = AF_INET;
  	destAddr.sin_len = sizeof (struct sockaddr_in);
  	destAddr.sin_addr = *dest;

  	memset(&gateAddr, 0, sizeof(gateAddr));
  	gateAddr.sin_family = AF_INET;
  	gateAddr.sin_len = sizeof (struct sockaddr_in);
  	gateAddr.sin_addr = *gateway;

  	memcpy (&routeEntry.rt_dst, &destAddr, sizeof(destAddr));
  	memcpy (&routeEntry.rt_gateway, &gateAddr, sizeof(gateAddr));

  	routeEntry.rt_flags |= (RTF_GATEWAY | RTF_UP | RTF_STATIC);
  	routeEntry.rt_flags |= flags;

  	sock = socket(AF_INET, SOCK_DGRAM, 0);
  	if (sock < 0)
  	{
      	perror("Error: ");
      	return 0;
   	}

	if (cmd == addRoute)
	{
  		if (ioctl(sock, SIOCADDRT, &routeEntry) < 0)
    		{
	   		close (sock);
			perror("Error: ");
			return 0;
    		}
    	}
    	else if (cmd == delRoute)
	{
  		if (ioctl(sock, SIOCDELRT, &routeEntry) < 0)
    		{
	   		close (sock);
			perror("Error: ");
			return 0;
    		}
	}	    	
    	
  	close (sock);

  	return 1;
}

int SendRouteMsg(in_addr* host, in_addr* gate)
{
	int 		sock;
 	struct sockaddr_in address;

	return 0;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		SysMessage(ERROR_MSG,"fail to create socket [%s]", strerror(errno));
		return 0;
	}

	address.sin_addr = *host;
	address.sin_port = htons(2005);
	address.sin_family = AF_INET;
	
	if (connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		SysMessage(ERROR_MSG,"fail to connect [%s]", strerror(errno));
		return 0;
	}

	send(sock, gate, sizeof(in_addr), 0);
	return 1;
}
