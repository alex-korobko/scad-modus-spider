/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __ESCALATOR_H__
#define __ESCALATOR_H__

#include <Pt.h>
#include <Ap.h>
#include <Ph.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/syspage.h>
#include <fcntl.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include "defines.h"
#include "cbuffer.h"

#define TYPE_PARAM   0
#define IP_PARAM   1
#define ID_PARAM   2
#define NUM_PARAM 3
#define DIR_PARAM   4

#define MODBUS_TCP_PORT 502
//интервал (в циклах) через котрый отсылается временная поправка
#define TIME_CORR_INTER 10000

typedef struct tagData
{
	word	status;
	word	mode;
	word ready;
	word BCstatus;
	word	BPstatus;
	word channels;
	word	reserved;
	word	msgCount;
	word	reserved1;
	dword breakingPath;
	dword odometr;
	dword reserved2;
	word signals[11];
	dword reserved3[2];
	word msg[8];
} DataBlock;
	
#define IMAGES_NUM	18

// escalator class
class metro_station;
class metro_escalator
{
public:
	// картинки индикаторов
   	enum {green_up, green_down, red_up, red_down, yellow_up, yellow_down, green_s_up, green_s_down, red_s_up, red_s_down, yellow_s_up, yellow_s_down, grey_stop, grey_off,
   				block_green_led, block_red_led, block_grey_led, block_blue_led};
   				
   	static PhImage_t* 	images[IMAGES_NUM];
	static int init_images;   	
   	static int		paneled;
   	static int 	reference;
   	static PtWidget_t*	panel;
   	
   	PtWidget_t* wnd;
   	PtWidget_t *Arrow, *numLabel, *blockLabel;
   	
   	int 			number;
   	int 			type;
   	dword 		id;

   	metro_station* parent;
   	pthread_t	tid;   
   	int				stationID;
	int				sleepticks;
   	int 			enabled;
    	int				online;
   	int				direction;
	int				blocked;
   	int				prefDirection;
   	word			prevCRC;   
    DataBlock 	dataBlock;
   	
   	Cbuffer_c	outBuffer;
   	Cbuffer_c	inBuffer;
   	in_addr_t	ip;
   	pthread_mutex_t		mutex;
   	pthread_mutex_t		dataMutex;
   	pthread_mutex_t		sendMutex;
   	int				conID;
   	int				chanID;
   	pid_t			pulse;
   	word 			oldMsg;
	
   
   	metro_escalator();
   	~metro_escalator();   
   	int CreateWnd(ApDBase_t* dbase, PtWidget_t* parent, int x = -1, int y = -1);
   	void UpdateEscalator();
   	void UpdatePanel(int force=0);
   	void SetParam(int param, int value);	
	void SetState(dword state);
	int SetData();
	int UpdateLeds();
	int ConnectToChannel(int channel);
	int DisconnectToChannel();
	int Start();
	dword GetID() { return id; }
   	int GetType() { return type; }
   	int SendCommand(byte cmd);   
   	int SendTime();   
   	int CheckStatus();
   	int GetData();
   	int GetNumber() { return number; }
};

void SetIndicator(PtWidget_t *widget, const char *text, int enable);
int ConnectToServer(in_addr_t addr);
int IsConnected(int sock, fd_set* rd, fd_set* wr, fd_set* ex);
int SendBuffer(int sock, byte* output, int size, byte* output);
void* Run(void* arg);

#endif

// that's all folks
