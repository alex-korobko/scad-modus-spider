/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 25.04.02
Last updated at 25.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/neutrino.h>
#include <sys/mman.h>


#include "global.h"
#include "modbus.h"
#include "system.h"
#include "lbuffer.h"
#include "routeinfo.h"
#include "watchdog.h"

RouteInfo	g_routeInfo;
ModBus_c	modbus;

extern int	 g_debugMode;

void* WatchDogThread (void* arg)
{
ThreadCtl(_NTO_TCTL_IO, 0);
uintptr_t pport_map;

pport_map=mmap_device_io(1, (uint64_t) WATCH_DOG_PORT);

if (pport_map<=0) {
	Log(ERROR, "Error creating watch dog timer thread");
	} else {
	WatchdogTimer WD_Timer(pport_map, WATCH_DOG_INTERVAL);	
	WD_Timer.Loop();
	};

return 0;
}


void* TestCommand(void* arg)
{
	byte	inBuffer[26];
	int		size;

		// передаем номер сокета для потока
		inBuffer[0] = 0;
		inBuffer[1] = 0; // кол-во перепосылок
		
		inBuffer[2] = 1;
		inBuffer[3] = 4;
		inBuffer[4] = 0;
		inBuffer[5] = 0;
		inBuffer[6] = 0;
		inBuffer[7] = 4;
		
		size=6;
		
		// рассчитываем CRC
		word crc  = CRC(&inBuffer[2], size);
		inBuffer[size+2] = HIBYTE(crc);
		inBuffer[size+3] = LOBYTE(crc);
		//Заглушки для соместимости
		inBuffer[size+4] = 0;
		inBuffer[size+5] = 0;


	do
	{
		usleep(9*100000);		
		if (g_debugMode)
		{
			printf("=====\nTest command: socket %d device N%d, command - %d\nBytes - ", inBuffer[0], inBuffer[2], inBuffer[3]);
			for(int i=0; i<size+4; i++)
				printf("0x%0x ", inBuffer[i]);
			printf("\n\n");
		}

		// посылаем команду
		if (!modbus.SendBuffer(inBuffer, size+4))
			Log(ERROR, "Output buffer overflow on sending");
	} while(1);

	return 0;
}



void* Process(void* arg)
{
	byte	inBuffer[264];
	int		size;

	int	sock = *(int*)arg;

	if (!sock)
	{
		Log(ERROR, "invalid socket parameter");
		return 0;
	}

	Log(DEBUG, "Start new ModBus thread with socket [%d]\n", sock);

	do
	{
		printf("Before read\n");
		// ждем заголовка (6 байт)
		if (!Receive(sock, inBuffer, 6))
		{
			close(sock);
			return 0;
		}
		// провер заголовок
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
		printf ("Calc CRC\n");		
		// рассчитываем CRC
		word crc  = CRC(&inBuffer[2], size);
		inBuffer[size+2] = HIBYTE(crc);
		inBuffer[size+3] = LOBYTE(crc);

		printf ("Calc Socket\n");		
		// передаем номер сокета для потока
		inBuffer[0] = sock;
		inBuffer[1] = 0; // кол-во перепосылок
		
		
		// посылаем команду
		if (!modbus.SendBuffer(inBuffer, size+4))
			{
			Log(ERROR,"Output buffer overflow on sending");
			printf ("Output buffer overflow on sending\n");
			} else {
			printf ("Data sended\n");
			};
	} while(1);

	return 0;
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
			else 	curGate = &routeInfo->gate1;
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


int main(int argc, const char *argv[])
{
	int		sock, curSock;
	struct	sockaddr_in address;
	size_t	addrLen;
	pthread_attr_t      attr;

	if (argc == 2)
	{
		if (!strcmp(argv[1], "-d"))
		{
			printf("Debug\n");
			g_debugMode = 1;
		}
			
	}

	if (!g_routeInfo.Load("/etc/route.cfg"))
	{
		Log(ERROR, "fail to load routing info");
		return 0;
	}
//Initialization Watch Dog Timer
	if (pthread_create(NULL, NULL, &WatchDogThread, NULL) != EOK)
		Log(ERROR, "fail to create watch dog thread [%s]", strerror(errno));



	if (pthread_create(NULL, NULL, &Router, &g_routeInfo) != EOK)
		Log(ERROR, "fail to create router thread [%s]", strerror(errno));

	// инициализация порта
	if (!modbus.InitPort(2, 115200))
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
/*
	if (g_debugMode) 
		{
			if (pthread_create(NULL, NULL, &TestCommand, NULL) != EOK)
			{
				Log(ERROR, "fail to create test command thread", strerror(errno));
				close(curSock);
			}
		};
*/
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

