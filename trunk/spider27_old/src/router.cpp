/*******************************************************************************
 Programming - Alexander Klimenko, Alexei Korobko
 Project - MetroNET, spider27_old
 Started at 27.12.00
 Last updated at 29.06.2003
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
int ChangeRoute(struct in_addr* dest, struct in_addr *gateway, int flags, int cmd);

Net::Net()
{
	ip = 0;
	curr_gateway_ip=0;
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
	struct in_addr tmp_IP1,tmp_IP2;
	printf("Load left leaf net\n");
	fd = fopen(filename, "r");
	if (fd)
	{
		fscanf(fd, "%d\n", &tableSize);		
		if (tableSize!=EOF)
		{
			for(int i=0; i<tableSize; i++)
			{	
				fscanf(fd, "%s\n", addr);
				printf("Load net %s\n", addr);
				net = new Net;
				net->ip = inet_addr(addr);
				net->curr_gateway_ip=leftGate.s_addr;

			 tmp_IP1.s_addr=net->ip;
			 tmp_IP2.s_addr=net->curr_gateway_ip;

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
	
	printf("Load right leaf net\n");
	
	fd = fopen(filename, "r");
	if (fd)
	{
		fscanf(fd, "%d\n", &tableSize);		
		if (tableSize!=EOF)
		{
			for(int i=0; i<tableSize; i++)
			{
				fscanf(fd, "%s\n", addr);
				printf("Load net %s\n", addr);
				net = new Net;
				net->ip = inet_addr(addr);
				net->curr_gateway_ip=rightGate.s_addr;
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


int ConnectToServer(in_addr_t addr, int port)
{
	int		sock;
	struct	sockaddr_in address;
	fd_set	readFD, writeFD, exFD;
	struct timeval timeout;
	int		flags;
	int		result;

	printf("\nIn connect to server\n");

	address.sin_addr.s_addr = addr;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;



	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (!sock)
	{
		printf( "fail to create sock [%s]", strerror(errno));
		return 0;
	}

	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
		printf ("fail to get file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		printf("fail to set file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
		printf("fail to connect [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			printf("fail to set file status flags [%s]", strerror(errno));
			close(sock);
			return 0;
		}
		
		return sock;
	}

	FD_ZERO(&readFD);
	FD_SET(sock, &readFD);
	writeFD = readFD;
	exFD = readFD;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	result = select(sock + 1, &readFD, &writeFD, &exFD, &timeout);
	if (result < 0)
	{
		printf("fail to select socket [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (!result)
	{
		printf("connect timeout expected [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (IsConnected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			printf("fail to set file status flags [%s]", strerror(errno));
			close(sock);
			return 0;
		}
		
		return sock;
	}
	else
	{
		close(sock);
		return 0;
	}
}


int Receive(int sock, byte* buffer, int size)
{
	int readNum;

	while (size > 0)
	{
    		readNum = recv(sock, buffer, 1, 0);
    		if (readNum < 0)
    		{
        		if (errno == EINTR)
            		continue;
        		printf ("fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		if (readNum == 0)
    		{
        		printf ("fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		buffer += readNum;
    		size -= readNum;
	}

	return 1;
}

void Router::Loop()
{
	Net*	curNet;
	char	buffer[8];
	int sock;
	in_addr_t host_addr;
	struct in_addr tmp_IP1, tmp_IP2;
	
 //  startup all hosts
	curNet = leftLine->next;
	printf("\nScan left\n");
	while(curNet!=NULL)
	{
		host_addr = (3 << 24) | curNet->ip;		

	 tmp_IP2.s_addr=host_addr;
// Begin: Printing 	current routing state
			printf("\n Change routing to host %s\n", inet_ntoa(tmp_IP2));
// End: Printing 	current routing state

		sleep(10);

		printf("\nScan left\n");
		
		if (!(sock = ConnectToServer(host_addr, 7)))
		{
			 tmp_IP1.s_addr=curNet->ip;
			 tmp_IP2.s_addr=curNet->curr_gateway_ip;
// Begin: Printing 	current routing state
			printf("\n Change routing to net %s ", inet_ntoa(tmp_IP1));
			printf(" trougth gateway %s (old left)\n",inet_ntoa(tmp_IP2));
// End: Printing 	current routing state
			ChangeRoute(&tmp_IP1, &tmp_IP2, 0, delRoute);
	
			if (curNet->curr_gateway_ip == leftGate.s_addr)
			   {
				curNet->curr_gateway_ip = rightGate.s_addr;
				}
			else { 
				curNet->curr_gateway_ip = leftGate.s_addr;
					};
			 tmp_IP1.s_addr=curNet->ip;
			 tmp_IP2.s_addr=curNet->curr_gateway_ip;

// Begin: Printing 	current routing state
			printf("\n Change routing to net %s ", inet_ntoa(tmp_IP1));
			printf(" trougth gateway %s (new left)\n",inet_ntoa(tmp_IP2));
// End: Printing 	current routing state

			ChangeRoute(&tmp_IP1, &tmp_IP2, 0, addRoute);
			if (!(sock = ConnectToServer(host_addr, 7))) { curNet->broken = 1; } ;
		} ;
		send(sock, "REQUEST", 7, 0);
	
		if (!Receive(sock, (byte*)buffer, 7)) { curNet->broken = 1;}  else { curNet->broken = 0;};
		
		buffer[7] = 0;
		close(sock);
		
		
		curNet = curNet->next;
	};


	curNet = rightLine->next;

	printf("\nScan right\n");
	while(curNet)
	{
		host_addr = (3 << 24) | curNet->ip;		
		
		sleep(10);

		if (!(sock = ConnectToServer(host_addr, 7)))
		{
			 tmp_IP1.s_addr=curNet->ip;
			 tmp_IP2.s_addr=curNet->curr_gateway_ip;

// Begin: Printing 	current routing state
			printf("\n Change routing to net %s ", inet_ntoa(tmp_IP1));
			printf(" trougth gateway %s (old right)\n",inet_ntoa(tmp_IP2));
// End: Printing 	current routing state

			 
			ChangeRoute(&tmp_IP1, &tmp_IP2, 0, delRoute);
	
			if (curNet->curr_gateway_ip == leftGate.s_addr)
			   {
				curNet->curr_gateway_ip = rightGate.s_addr;
				}
			else { 
				curNet->curr_gateway_ip = leftGate.s_addr;
					};
			 tmp_IP1.s_addr=curNet->ip;
			 tmp_IP2.s_addr=curNet->curr_gateway_ip;

// Begin: Printing 	current routing state
			printf("\n Change routing to net %s ", inet_ntoa(tmp_IP1));
			printf(" trougth gateway %s (new right)\n",inet_ntoa(tmp_IP2));
// End: Printing 	current routing state


			ChangeRoute(&tmp_IP1, &tmp_IP2, 0, addRoute);
			if (!(sock = ConnectToServer(host_addr, 7))) { curNet->broken = 1; } ;
		} ;
		send(sock, "REQUEST", 7, 0);
	
		if (!Receive(sock, (byte*)buffer, 7)) { curNet->broken = 1;}  else { curNet->broken = 0;};
		
		buffer[7] = 0;
		close(sock);
		
		
		curNet = curNet->next;
	};

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

