/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 01.04.02
Last updated at 16.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#include <sys/syspage.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <sys/neutrino.h>

#include "global.h"
#include "system.h"
#include "modbus.h"
#include "list.h"
#include "lbuffer.h"


#define CHECK_ADDRESS(addr) if (addr > 247) { Log(ERROR, "Incorrect device address (%d)", addr); return 0; }
#define CHECK_ORDER(lo, hi)	if (hi < lo) { Log(ERROR, "Last address of requested block less then first"); return 0; }

CmdPool 				outBuffer;
pthread_mutex_t 	mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  		condvar = PTHREAD_COND_INITIALIZER;
int dataReady = 0; // переменная состояния (готовность отправки данных)
const int responseTime = 1000; // время ожидания ответа от устройства (мсек.)
const int requestPause = 50000; // мксек
const int resendTimes = 2;

extern int g_debugMode;

void* Receiver(void* arg);

void SendNoResponse(int sock, byte addr, byte cmd);

ModBus_c::ModBus_c()
{
	port = 0;    
	portHandle = 0;
	portSpeed = 0;
}

ModBus_c::~ModBus_c()
{
	// закрываем читающий поток
	if (pthread_cancel(tid) != EOK)
		Log(ERROR, "Fail to close data receiver thread");

	// закрываем СОМ-порт
	if (portHandle)
		close(portHandle);
}

int ModBus_c::SendBuffer(byte* buffer, int size)
{
	int		exitCode;
	CHECK_ADDRESS(buffer[1]);

	// "взводим" переменную состояния (есть данные для посылки)
	
	pthread_mutex_lock (&mutex);
	exitCode = outBuffer.Put(buffer, size);
	pthread_mutex_unlock (&mutex);
	
	return exitCode;
}

int ModBus_c::Run()
{
	// оздаем поток для чтения из СОМ-порт
	if (pthread_create(&tid, NULL, &Receiver, this) != EOK)
	{
		Log(ERROR, "Fail to create thread");
		return 0;
	}

	return 1;
}

int ModBus_c::InitPort(int portNum, int speed)
{
	char	devPath[12];
	struct termios termiosParam;

	port = portNum;

	// проверям корректность номера порта
	if (port < 1 || port > 4)
		return 0;

	// создаем имя порта (Unix namespace)
	sprintf(devPath, "/dev/ser%d", port);
	
	// открываем...
	portHandle = open(devPath, O_RDWR);
	if (portHandle < 0)
	{
	    Log(ERROR, "fail to open COM-%d", port);
	    return 0;
	}

	// устанавливаем скорость чтения/записи
    	portSpeed = speed;

	tcgetattr(portHandle, &termiosParam);
	if (cfsetispeed(&termiosParam, speed ) < 0)
	{
		Log(ERROR, "fail to setup input speed on COM%d", port);
		close(portHandle);
		portHandle = port = 0;		
		return 0;
	}
	if (cfsetospeed(&termiosParam, speed ) < 0)
	{
		Log(ERROR, "Fail to setup input speed on COM%d", port);
		close(portHandle);
		portHandle = port = 0;
		return 0;
	}
	
	tcsetattr(portHandle, TCSADRAIN, &termiosParam);

	// очищаем буфферы			
    	tcflush(portHandle, TCIOFLUSH);

	return 1;		
}

void* Receiver(void* arg)
{
	ModBus_c*	device = NULL;
	int				port;
	fd_set 			allFd, readFd;
	int				result;
	int				count;
	int				i, j;
	struct timeval	silenceTimeout;
	struct timeval	receiveTimeout;
	struct timeval	responseTimeout;
	double			timeout;
	word			crc;
	byte			output[MODBUS_BUFFER_SIZE];
	byte			input[MODBUS_BUFFER_SIZE];
	int				size = 0;
	int				noResponse = 0;
/* Начало переменные для учета времени */
	uint64_t before=0,after=0, cps = SYSPAGE_ENTRY( qtime )->cycles_per_sec;
/* Конец переменные для учета времени */
/*Начало - отладочная переменная */
	int debug_cou;
/*Конец - отладочная переменная */
	// если не передан указатель на ModBus_c - до свиданья
	if (!arg)
	{
		Log(ERROR, "pointer to ModBus intstance not initialized");
		return 0;
	}

	device = (ModBus_c*)arg;

	// наш СОМ-
	port = device->GetPort();
	if (!port)
	{
		Log(ERROR, "not initialized COM-port expected");
		return 0;
	}

	// устанавливаем наш порт в "очередь" на чтее
	FD_ZERO(&allFd);
	FD_SET(port, &allFd);

	// время передчи одного смвола
	timeout = 10.0/device->GetSpeed();

	// минимальный таймаут между посылками
	silenceTimeout.tv_sec = (int)(timeout*3.5);
	silenceTimeout.tv_usec = (int)((timeout*3.5 - silenceTimeout.tv_sec)*1000000);


	// максимальный таймаут приема между двумя соседними байтами в посылке
//	receiveTimeout.tv_sec = (int)(timeout*1.5);
//	receiveTimeout.tv_usec = (int)((timeout*1.5 - receiveTimeout.tv_sec)*1000000);

	receiveTimeout.tv_sec = (int)(timeout*1.5*5);
	receiveTimeout.tv_usec = (int)((timeout*1.5 - receiveTimeout.tv_sec)*1000000 *10);

	
	// максимальное время ожидания ответа от устройства
//	responseTimeout.tv_sec = 1;
//	responseTimeout.tv_usec = responseTime*1000;	
	
	responseTimeout.tv_sec = 8;
	responseTimeout.tv_usec = responseTime*1000;	

	while(1)
	{
		if (noResponse)
		{
			noResponse = 0;
			SendNoResponse(output[0], output[2], output[3]);
		}

		// делаем паузу между запросами
		usleep(requestPause);

		// ждем, когда в буффере появятся данные для отправки
		pthread_mutex_lock (&mutex);
		size = outBuffer.Get(output, MODBUS_BUFFER_SIZE);
		pthread_mutex_unlock (&mutex);

		if (!size)
			continue;

		if (output[1] > resendTimes)
			continue;

		if (g_debugMode)
		{
			printf("Send modbus packet (size %d): ", size);
			for(j=0; j<size; j++)
				printf("0x%0X ", output[j]);
			putchar('\n');
			putchar('\n');
		}

		// выжидаем паузу между пакетами
		readFd = allFd;
		do
		{
			if (g_debugMode) {before=ClockCycles();};
			result = select(port + 1, &readFd, NULL, NULL, &silenceTimeout);
			if (g_debugMode) {after=ClockCycles();};
			
			if (g_debugMode) {
			printf("Time in select (pause brfore write)\t%f\n", (float) (after-before)/cps);			
										};
			
			if (result < 0)
			{
				Log(ERROR, "fail to select on port. %s\n", strerror(errno));
				break;
			}
			else if (result > 0)
			{
				// если что-то есть в буффере - очистить
				tcflush(port, TCIOFLUSH);
				Log(DEBUG, "Flush garbage into COM-port\n");
			}
			
		} while (result > 0);

		if (result < 0)
		{
			noResponse = 1;
			continue;
		}

		// сбрасывае счетчик байт во ходящем бффере
		// и заголовок
		memset(input, 0, 6);

		// посылаем пакет
		write(port, &output[2], size-2);
	
//		if (tcdrain(port)<0) {Log(ERROR,"In tcdrain [%s]",strerror(errno));};
		if (g_debugMode) {before=ClockCycles();};
		usleep((int)((size-2)*timeout*1000000));
		if (g_debugMode) {after=ClockCycles();};

		if (g_debugMode) {
			printf("Time in usleep (pause after write before read)\t%f\n", (float) (after-before)/cps);			
										};
		


		tcflush(port, TCIOFLUSH);
		
		readFd = allFd;
		if (g_debugMode) {before=ClockCycles();};
		result = select(port + 1, &readFd, NULL, NULL, &responseTimeout);
		if (g_debugMode) {after=ClockCycles();};

		if (g_debugMode) {
			printf("Time in select (reading first byte)\t%f\n", (float) (after-before)/cps);			
										};
		

		
		if (result && FD_ISSET(port, &readFd))
//			read(port, &input[6], 1);	
			readcond(port, &input[6], 1, 1, 5, 15);	
			
		else if (!result)
		{
			if (g_debugMode)	printf("Not response...\n");

			output[1]++;
			if (g_debugMode)
				printf("Resend packet [%d]\n",output[1]); 
			device->SendBuffer(output, size);			
			continue;
		}
		else
		{
			noResponse = 1;
			Log(ERROR, "Fail to select on port. %s", strerror(errno));
			continue;
		}

		if (g_debugMode) {printf("1 byte 0x%x\n", input[6]);};			
				
		// минимально вычитваем три байта
		count = 3;
		for(i=1; i<count; i++)
		{
			readFd = allFd;
			if (g_debugMode) {before=ClockCycles();};
			result = select(port + 1, &readFd, NULL, NULL, &receiveTimeout);
			if (g_debugMode) {after=ClockCycles();};

            if (g_debugMode) {
			printf("Time in select reading %d byte\t%f\n", i,(float) (after-before)/cps);			
										};
			//Убрать!!!		
			debug_cou=i;
			if (result && FD_ISSET(port, &readFd))
			{
//				read(port, &input[i+6], 1);
				readcond(port, &input[i+6], 1, 1, 5, 15);
				// если это третий байт, то это число байт в ответе
				if (i == 2)
				{
					// опрдеяем размер остатка входящего пакета
					// если установлен старший бит - вернули ошибку (пакет = 5 байт)
						if (input[i+5] & 0x80)
						count = 5;
					// иначе вернули нормальный ответ
					else
					{
						switch(input[i+5])
						{
							case READ_OUTPUT_STATUS:
							case READ_INPUT_STATUS:
							case READ_HOLDING_REGISTERS:
							case READ_INPUT_REGISTERS:
								count = input[i+6] + 5;
								break;
							case FORCE_SINGLE_COIL:
							case PRESET_SINGLE_REGISTER:
							case FORCE_MULTIPLE_COILS:
							case PRESET_MULTIPLE_REGISTERS:
								count = 8;
								break;
							default:
								break;
						}
					}
					printf("Try to receive %d bytes\n", count);
				}							
			}
			else if (!result)
			{
				output[1]++;
				if (g_debugMode)
						{
						printf("Packets fragment (size - %d): ", debug_cou);
						i=0;
						for(j=6; j<debug_cou+6; j++) {
										if (++i>=10) {
												  i=0;
												  printf ("\n");
													};

						printf("0x%0X\t\t", input[j]);
						}
						putchar('\n');
						putchar('\n');
						
						printf("Resend packet\n");
						};
				device->SendBuffer(output, size);				
				break;
			}
			else
			{
				Log(ERROR, "fail to select on port [%s]", strerror(errno));
				noResponse = 1;
				break;
			}
		}

		// если из цикла приема вышли с ошибкой - все с начала
		if (result <= 0)
			continue;

		// проверяем CRC
		crc = CRC(&input[6], i-2);

		if (g_debugMode)
		{
			printf("Receive packet (size - %d): ", i);
			debug_cou=0;
			for(j=6; j<count+6; j++)
				{
				if (++debug_cou>=10) {
												  debug_cou=0;
												  printf ("\n");
													};
				printf("0x%0X\t\t", input[j]);
				};
			putchar('\n');
			putchar('\n');
		}

		if ((HIBYTE(crc) == input[i+4]) && (LOBYTE(crc) == input[i+5]))
		{
			// устанвливам кол-во байт в приемном буффере
			input[5] = i - 2;
			printf("Send packet socket %d [%d]\n", output[0], input[6]);
			send(output[0], input, input[5] + 6, 0);
		}
		else
		{
			Log(ERROR, "bad packet CRC");
			output[1]++;
		}
	}
}

int ModBus_c::SetRTS(int state)
{
	int data;

	state? data = _CTL_RTS_CHG | _CTL_RTS: data = _CTL_RTS_CHG | 0;

	if (devctl (portHandle, DCMD_CHR_SERCTL, &data, sizeof(data), NULL))
		return 0;
	else
		return 1;
}

void SendNoResponse(int sock, byte addr, byte cmd)
{
	byte	buffer[9];

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 3;
	buffer[6] = addr;
	buffer[7] = cmd | 0x80;
	buffer[8] = 0x0B;
	if (send(sock, buffer, 9, 0) <= 0)
		Log(ERROR, "fail to send packet [%s]", strerror(errno));
}
