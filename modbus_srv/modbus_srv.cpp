/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 25.04.02
Last updated at 25.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "global.h"
#include "system.h"
#include "modbus.h"
#include "lbuffer.h"

class RouteInfo
{
public:
	in_addr					gate1, gate2;
	in_addr_t 				master;
	RouteInfo();
	~RouteInfo();
	int		Load(const char*	fileName);
};

RouteInfo	g_routeInfo;
ModBus_c	modbus;
int		g_debugMode = 0, port_number=2;
enum {addRoute, delRoute};

RouteInfo::RouteInfo()
{
	gate1.s_addr = 0;
	gate2.s_addr = 0;
	master = 0;
}

RouteInfo::~RouteInfo()
{
}

int RouteInfo::Load(const char* fileName)
{
	FILE*		file;
	char		buffer[20];
	
	file = fopen(fileName, "rt");
	if (!file)
	{
		Log(ERROR, "fail to open routing info file '%s'", fileName);
		return 0;
	}
		
	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		master = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}

	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		gate1.s_addr = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}

	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		gate2.s_addr = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}
	
	fclose(file);
	return 1;
}

int IsConnected(int sock, fd_set* rd, fd_set* wr, fd_set* ex)
{
	int err;
	size_t 	length = sizeof(err);

	if (!FD_ISSET(sock, rd) && !FD_ISSET(sock, wr))
		return 0;

	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &length) < 0)
		return 0;
	errno = err;
	
	return err == 0;
}

int ConnectToServer(in_addr_t addr, int port)
{
	int		sock;
	struct	sockaddr_in address;
	fd_set	readFD, writeFD, exFD;
	struct timeval timeout;
	int		flags;
	int		result;

	address.sin_addr.s_addr = addr;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (!sock)
	{
		Log(ERROR, "fail to create sock [%s]", strerror(errno));
		return 0;
	}

	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
		Log(ERROR, "fail to get file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		Log(ERROR, "fail to set file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
		Log(ERROR, "fail to connect [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			Log(ERROR, "fail to set file status flags [%s]", strerror(errno));
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
		Log(ERROR, "fail to select socket [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (!result)
	{
		Log(ERROR, "connect timeout expected [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (IsConnected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			Log(ERROR, "fail to set file status flags [%s]", strerror(errno));
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
/*
Commented by kor 17.02.2003
*/
/*
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
        		Log(ERROR, "fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		if (readNum == 0)
    		{
        		Log(ERROR, "fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		buffer += readNum;
    		size -= readNum;
	}

	return 1;
}
*/

/* 
New version  (righter?) from kor
17.02.2003
*/

int Receive(int sock, byte* buffer, int init_size)
{
	int readNum,i,size;
	byte tmp_buff[20];
	
	size=init_size;
	
	while (size > 0)
	{
    		readNum = recv(sock, tmp_buff, 1, 0);
    		if (readNum < 0)
    		{
        		if (errno == EINTR)
            		continue;
        		Log(ERROR, "fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		if (readNum == 0)
    		{
        		Log(ERROR, "fail to receive data [%s]", strerror(errno));
        		return 0;
    		}
    		
		for (i=0; i<readNum; i++)
		{
    		buffer[init_size-size+i] += tmp_buff[i];
		};
		    		
    		size -= readNum;
	}

	return 1;
}


void* Process(void* arg)
{
	byte	inBuffer[264];
	int		size;

	int 	sock = *(int*)arg;

	if (!sock)
	{
		Log(ERROR, "invalid socket parameter");
		return 0;
	}

	Log(DEBUG, "Start new ModBus thread with socket [%d]\n", sock);

	do
	{
		// ждем заголовка (6 байт)
		if (!Receive(sock, inBuffer, 6))
		{
			close(sock);
			return 0;
		}
		// проверяем заголовок
		if (inBuffer[0] || inBuffer[1] || inBuffer[2] || inBuffer[3] || inBuffer[4])
		{
			close(sock);
			return 0;
		}
		// размер пакета
		size = inBuffer[5];
		if (!Receive(sock, &inBuffer[2], size))
		{
			close(sock);			
			return 0;
		}
		
		if (g_debugMode)
		{
			printf("#####\nGot a command: socket %d device N%d, command - %d\nBytes - ", sock, inBuffer[2], inBuffer[3]);
			for(int i=2; i<size+2; i++)
				printf("0x%0x ", inBuffer[i]);
			printf("\n\n");
		}
		
		// рассчитываем CRC
		word crc  = CRC(&inBuffer[2], size);
		inBuffer[size+2] = HIBYTE(crc);
		inBuffer[size+3] = LOBYTE(crc);

		// передаем номер сокета для потока
		inBuffer[0] = sock;
		inBuffer[1] = 0; // кол-во перепосылок

		// Посылаем команду
		if (!modbus.SendBuffer(inBuffer, size+4))
			Log(ERROR, "Output buffer overflow on sending");
	} while(1);

	return 0;
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

void* Router(void* arg)
{
	int		sock;
	char	buffer[8];
	in_addr		defaultNet;
	RouteInfo*	routeInfo = NULL;
	
	if (!arg)
	{
		Log(ERROR, "zero routing info pointer");
		return 0;
	}
	
	routeInfo = (RouteInfo*)arg;
	
	defaultNet.s_addr = 0;
	in_addr*	curGate = &routeInfo->gate1;

	do
	{
		sleep(15);
		if (!(sock = ConnectToServer(routeInfo->master, 7)))
		{
			if (curGate == &routeInfo->gate1)
				curGate = &routeInfo->gate2;
			else if (curGate == &routeInfo->gate2)
				curGate = &routeInfo->gate1;
			Log(INFO, "Change routing");
			ChangeRoute(&defaultNet, &defaultNet, 0, delRoute);
			ChangeRoute(&defaultNet, curGate, 0, addRoute);
			if (!(sock = ConnectToServer(routeInfo->master, 7)))
			{
				Log(ERROR,"fail to connect");
				continue;
			}
		}

		send(sock, "REQUEST", 7, 0);
	
		if (!Receive(sock, (byte*)buffer, 7))
		{
			Log(ERROR, "No response from echo server");
			close(sock);
			continue;
		}
		buffer[7] = 0;
		close(sock);
	} while(1);

	return 0;
}

void usage (const char *file_name)
{
printf ("\nUsage :\n\t%s COM_port_number [-d]\n\twhere:\n\tCOM_port_number - number of COM port, used for Modbus protocol\n\t-d - turn on debug mode\n",file_name);
}

int main(int argc, const char *argv[])
{
	int		sock, curSock;
	struct	sockaddr_in address;
	size_t	addrLen;
	pthread_attr_t      attr;
	
	if (argc >= 2)
	{
	if (isdigit(argv[1][0]) && atoi(argv[1])>0 && atoi(argv[1])<5)
	{
	port_number=atoi(argv[1]);
	} else {
	    printf("Wrong first parameter - COM port number.\n");
	    usage(argv[0]);
	    return -1;
	}
	    
	 if (argc >= 3)
	 {
		if (!strcmp(argv[2], "-d"))
		{
			printf("Debug\n");
			g_debugMode = 1;
		}
			
	  }
	}  else {
	    printf("Wrong first parameter - COM port number.\n");
	    usage(argv[0]);
	    return -1;
	}
	
	if (!g_routeInfo.Load("/etc/route.cfg"))
	{
		Log(ERROR, "fail to load routing info");
		return 0;
	}

	if (pthread_create(NULL, NULL, &Router, &g_routeInfo) != EOK)
	
		Log(ERROR, "fail to create router thread [%s]", strerror(errno));

	// инициализация порта
	if (!modbus.InitPort(port_number, 115200))
	{
		Log(ERROR, "fail to initialize ModBus module");
		return -1;
	}

	// запуск ModBus-потока
	if (!modbus.Run())
	{
		Log(ERROR, "fail to start ModBus thread");
		return -1;
	}

	// открываем порт на прослушивание запросов
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Log(ERROR,"fail to create 'listen' socket [%s]", strerror(errno));
		return -1;
	}

	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(MODBUS_PORT);
	address.sin_family = AF_INET;
	
	if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		Log(ERROR,"fail to bind socket [%s]", strerror(errno));
		close(sock);
		return -1;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	listen(sock, 15);
	do
	{
		addrLen = sizeof(address);
		curSock = accept(sock, (struct sockaddr*)&address, &addrLen);
		if (curSock < 0)
		{
			Log(ERROR,"fail to accept socket [%s]", strerror(errno));
		}
		else
		{
			if (pthread_create(NULL, &attr, &Process, &curSock) != EOK)
			{
				Log(ERROR, "fail to create device thread [%s]", strerror(errno));
				close(curSock);
			}
		}
	} while(1);

	return 0;
}

