/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 16.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "types.h"

#define LOBYTE(data)	(data & 0x00FF)
#define HIBYTE(data)	(data & 0xFF00) >> 8

enum {ERROR, INFO, DEBUG};
enum {addRoute, delRoute};
	
void Log(int type, const char* message, ...);
word CRC(const byte* buffer, int size);
int IsConnected(int sock, fd_set* rd, fd_set* wr, fd_set* ex);
int ConnectToServer(in_addr_t addr, int port);
int Receive(int sock, byte* buffer, int size);
int ChangeRoute(struct in_addr* dest, struct in_addr *gateway, int flags, int cmd);

#endif
