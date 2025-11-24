/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 01.04.02
Last updated at 16.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/neutrino.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include "types.h"

#define MODBUS_BUFFER_SIZE	512

enum { PULSE_OK = _PULSE_CODE_MINAVAIL + 1,
		PULSE_NO_RESPONSE,
		PULSE_TIMEOUT_EXPECT,
		PULSE_BAD_CRC,
		PULSE_ILLEGAL_FUNCTION,
		PULSE_ILLEGAL_DATA_ADDR,
		PULSE_ILLEGAL_DATA_VAL,
		PULSE_FAILURE_IN_DEVICE,
		PULSE_ACKNOWLEDGE,
		PULSE_DEVICE_BUSY,
		PULSE_NAK_NEGATIVE,
		PULSE_UNKNOWN_ERROR
 };

enum { ILLEGAL_FUNCTION = 1,
		ILLEGAL_DATA_ADDRESS,
		ILLEGAL_DATA_VALUE,
		FAILURE_IN_DEVICE,
		ACKNOWLEDGE,
		BUSY,
		NAK_NEGATIVE };

enum {COM1, COM2, COM3, COM4};

enum {
	READ_OUTPUT_STATUS = 0x01,
	READ_INPUT_STATUS,
	READ_HOLDING_REGISTERS,
	READ_INPUT_REGISTERS,
	FORCE_SINGLE_COIL,
	PRESET_SINGLE_REGISTER,
	FORCE_MULTIPLE_COILS = 0x0F,
	PRESET_MULTIPLE_REGISTERS
};

class ModBus_c
{
protected:
	int		port;
	int		portHandle;
	int		portSpeed;
	pthread_t   tid;

public:
	ModBus_c();
	virtual ~ModBus_c();
	int Run();
	int  SendBuffer(byte* buffer, int size);
	int InitPort(int portNum, int speed);
	int GetPort() { return portHandle; }
	int GetSpeed() { return portSpeed; }
	int SetRTS(int state);
};

void* Receiver(void* arg);
void SendNoResponse(int sock, byte addr, byte cmd);

#endif
