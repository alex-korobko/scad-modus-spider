/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <Ph.h>
#include <time.h>
#include "escalator.h"
#include "abimport.h"
#include "global.h"
#include "system.h"
#include "log.h"

#define	NO_BLOCK_COLOR		0x606060
#define	BLOCK_COLOR		0xFFC000
#define	RECEIVE_TIMEOUT		1

#define CHECK_ADDRESS(addr) if (addr > 247) { SysMessage(ERROR_MSG, "Incorrect device address (%d)", addr); return 0; }
#define CHECK_ORDER(lo, hi)	if (hi < lo) { SysMessage(ERROR_MSG, "Last address of requested block less then first"); return 0; }

PtWidget_t*	metro_escalator::panel = NULL;
pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;
metro_escalator* g_selectedEscalator = NULL;
int	g_selectDirection = -1;
metro_escalator*	g_paneledEsc = NULL;
int	metro_escalator::paneled = 0;
int metro_escalator::reference = 0;
int metro_escalator::init_images = 0;
//int     metro_escalator::paneled = 0;
//int metro_escalator::reference = 0;

#define	STATUS_WORK			7

char *mode_text[] = { "ГП со щита",
	"ГП с пульта",
	"ГП с ДУ",
	"МП со щита",
	"МП с пульта",
	"Растормаж.",
	"Тест" };
	

char *ready_text[] = { "ГП готов",
	"МП Готов",
	"Тест готов",
	"Раст. готов",
	"Выбег готов",
	"Не готов",
	"",
	"Работает" };
	
PhImage_t* metro_escalator::images[IMAGES_NUM];
//PtWidget_t* metro_escalator::cursor = NULL;
int led_count, ledRowCount;
metro_escalator* curEscalator = NULL;
metro_escalator* control = NULL;



PtWidget_t *InflateBalloon(PtWidget_t *window,
							PtWidget_t *widget,
							int position,
							char *text,
							char *font,
							PgColor_t fill_color,
							PgColor_t text_color );

metro_escalator::metro_escalator()
{
	wnd = NULL;
	parent = NULL;
	Arrow = numLabel = blockLabel = NULL;
	number = 0;
	enabled = 0;
	type = 0;
	id = 0;
	direction = 0;
	prefDirection = 0;
	prevCRC = 0;
	reference++;
//	memset(signals, 0, sizeof(PtWidget_t*)*12*8);

	blocked = 0;	
	online = -1;

	inBuffer.Create(1024);
	conID = 0;	
	pulse = 0;
	
	oldMsg = 0;
	sleepticks = 0;
	pthread_mutex_init(&mutex, NULL);	
	pthread_mutex_init(&dataMutex, NULL);
	pthread_mutex_init(&sendMutex, NULL);		

	stationID = -1;
}

int ConfirmDirection()
{
	char* btns[2];
	unsigned long	*flags;
	int RetVal;

	
	char Helvetica14[MAX_FONT_TAG];

	btns[0] = ApGetTextRes(widget_dbase, "@yes");
	btns[1] = ApGetTextRes(widget_dbase, "@no");
	

	RetVal=PtAlert( ABW_main_wnd, NULL, ApGetTextRes(widget_dbase, "@warning"), NULL, ApGetTextRes(widget_dbase, "@changeDirection"),
	           (char*)PfGenerateFontName((uchar_t*)"TextFont", PF_STYLE_ANTIALIAS | PF_STYLE_BOLD, 11, (uchar_t*)Helvetica14), 2, (const char**)btns, NULL, 1, 2, Pt_BLOCK_ALL ) ;
   	
	return (RetVal);
}

metro_escalator::~metro_escalator()
{
	if (pthread_cancel(tid) != EOK)
	{
		SysMessage(ERROR_MSG, "Fail to close escalator's thread");
	}
	pthread_mutex_destroy(&sendMutex);
	pthread_mutex_destroy(&dataMutex);
	pthread_mutex_destroy(&mutex);
	
	reference--;
}

int metro_escalator::CreateWnd(ApDBase_t* dbase, PtWidget_t* parent, int x,	int y)
{
	PtArg_t args[2];
	char    label_buffer[4];
	
	if (!init_images)
	{
		images[0] = ApGetImageRes(dbase, "green_up");
		images[1] = ApGetImageRes(dbase, "green_down");	
		images[2] = ApGetImageRes(dbase, "red_up");
		images[3] = ApGetImageRes(dbase, "red_down");
		images[4] = ApGetImageRes(dbase, "yellow_up");
		images[5] = ApGetImageRes(dbase, "yellow_down");
		images[6] = ApGetImageRes(dbase, "green_s_up");
		images[7] = ApGetImageRes(dbase, "green_s_down");
		images[8] = ApGetImageRes(dbase, "red_s_up");		
		images[9] = ApGetImageRes(dbase, "red_s_down");		
		images[10] = ApGetImageRes(dbase, "yellow_s_up");
		images[11] = ApGetImageRes(dbase, "yellow_s_down");
		images[12] = ApGetImageRes(dbase, "grey_stop");		
		images[13] = ApGetImageRes(dbase, "grey_off");		
		images[14] = ApGetImageRes(dbase, "block_green_led");		
		images[15] = ApGetImageRes(dbase, "block_red_led");		
		images[16] = ApGetImageRes(dbase, "block_yellow_led");		
		images[17] = ApGetImageRes(dbase, "block_blue_led");								
  	};	
	
    PtSetParentWidget(parent);
	PtSetArg(&args[0], Pt_ARG_POINTER, this, 0);
    wnd = ApCreateWidget(dbase, "escalator_wnd", x, y, 1, args);
    	
    if (!wnd)   		
		return 0;
	PtSetArg(&args[0], Pt_ARG_POINTER, this, 0);   
	Arrow = ApCreateWidget(dbase, "Arrow", -1, -1, 1, args);
    if (!Arrow)
		return 0;
		
	
	ltoa(number, label_buffer, 10);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, label_buffer, 0);
	numLabel = ApCreateWidget(dbase, "escalator_num", -1, -1, 1, args);


    if (!numLabel)
		return 0;
	
					
	PtSetArg(&args[0], Pt_ARG_COLOR, NO_BLOCK_COLOR, 0);
	PtSetArg(&args[1], Pt_ARG_POINTER, this, 0);	


	blockLabel = ApCreateWidget(dbase, "blocking", -1, -1, 2, args);
    if (!blockLabel)
		return 0;
	
	UpdateEscalator();

	return 1;
}

void metro_escalator::UpdateEscalator()
{
	PtArg_t args[2];

	if (enabled)
	{
		printf("Enabled\n");
		if (online == 1)
		{
			printf("Online mode %d %d\n", dataBlock.mode, dataBlock.status);
			if (dataBlock.mode == 3)
			{
				switch(dataBlock.status)
				{		
					case 2:
					case 4:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[green_up], 0);
						break;
					case 6:
						if (prefDirection == 1)
							{
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_up], 0);
						    	}
						else if (prefDirection == 2)
							{
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_down], 0);					
						    	}
						else if (prefDirection == 3)
						{
							if (direction == DIRECTION_UP)
								{
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_up], 0);
								}
							else if (direction == DIRECTION_DOWN)
								{
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_down], 0);							
								}
						}
						break;
					case 3:
					case 5:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[green_down], 0);
						break;
					default:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[grey_stop], 0);
						break;
				}
			}
			else
			{
				switch(dataBlock.status)
				{		
					case 2:
					case 4:		
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[green_s_up], 0);
						break;
					case 6:		
						if (prefDirection == 1)
							{
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_s_up], 0);
							}
						else if (prefDirection == 2)
							{
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_s_down], 0);						
							}
						else if (prefDirection == 3)
						{
							if (direction == DIRECTION_UP)
								{
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_s_up], 0);
								}
							else if (direction == DIRECTION_DOWN)
								{
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[yellow_s_down], 0);							
								}
						}
						break;
					case 3:
					case 5:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[green_s_down], 0);
						break;
					default:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[grey_stop], 0);
						break;
				}
			}	
			PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
			PtSetResources(Arrow, 2, args);		
		}
		else
		{

				PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[grey_stop], 0);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
				PtSetResources(Arrow, 2, args);		
		}
	}
	else
	{
		PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[grey_off], 0);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
		PtSetResources(Arrow, 2, args);		
	}
}

void metro_escalator::UpdatePanel(int force)
{
	PtArg_t args[1];
	
	if (paneled)
	{
		if (online)
		{			
			if (dataBlock.mode > 0 && dataBlock.mode <=7 )
			{
				SetIndicator(ABW_ModeIndicator, mode_text[dataBlock.mode - 1], 1);
			}
			if ((dataBlock.ready >= 0) && (dataBlock.ready <8))
			{	
				switch(dataBlock.status)
				{		
					case 2:
					case 3:
					case 4:
					case 5:
						SetIndicator(ABW_ReadyIndicator, ready_text[STATUS_WORK], 1);
						break;
					default:
						SetIndicator(ABW_ReadyIndicator, ready_text[dataBlock.ready], 1);
						break;
				}
			}
		}
	}
}

void metro_escalator::SetParam(int param, int value)
{
	switch(param)
	{
	case TYPE_PARAM:
		type = value;
		break;
	case ID_PARAM:
		id = (dword)value;
		break;
	case NUM_PARAM:
		number = (int)value;
		break;
	case DIR_PARAM:
		prefDirection = (int)value;
		break;
	case IP_PARAM:
		ip = inet_addr((char*)value);
		printf("Load %s\n", ip);
		break;
	default:
		break;
	}
}

void SetIndicator(PtWidget_t* widget, const char *text, int enable)
{
	PtArg_t args[3];
	char buffer[128];
	
	if (enable)
	{		
		//translate_string(text, buffer, translate_set);
		PtSetArg(&args[0], Pt_ARG_FILL_COLOR, 0x00747E92, 0);
		PtSetArg(&args[1], Pt_ARG_COLOR, 0x0000FF00, 0);
		PtSetArg(&args[2], Pt_ARG_TEXT_STRING, text, 0);
		PtSetResources(widget, 3, args);
	}
	else
	{		
		PtSetArg(&args[0], Pt_ARG_FILL_COLOR, 0x00BEBEBE, 0);
		PtSetArg(&args[1], Pt_ARG_COLOR, 0x00646464, 0);
		PtSetArg(&args[2], Pt_ARG_TEXT_STRING, "", 0);
		PtSetResources(widget, 3, args);		
	}	
}

void enable_header(PtWidget_t* widget, int enable)
{
	PtArg_t args[3];
		
	if (enable)
	{		
		PtSetArg(&args[0], Pt_ARG_FILL_COLOR, 0x00B1B8D1, 0);
		PtSetArg(&args[1], Pt_ARG_COLOR, 0x001D284A, 0);
		PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetResources(widget, 3, args);
	}
	else
	{
		PtSetArg(&args[0], Pt_ARG_FILL_COLOR, 0x00BEBEBE, 0);
		PtSetArg(&args[1], Pt_ARG_COLOR, 0x00646464, 0);
		PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
		PtSetResources(widget, 3, args);		
	}	
}

PtWidget_t *InflateBalloon(PtWidget_t *window,
							PtWidget_t *widget,
							int position,
							char *text,
							char *font,
							PgColor_t fill_color,
							PgColor_t text_color )
{
	printf("Text %s\n", text);
	return widget;
}

int metro_escalator::SetData()
{
	byte		buffer[128];
	int 		result;
	word		msg;	
	PtArg_t	args[1];

//	printf("Set esc data\n");
	
	pthread_mutex_lock(&dataMutex);
	result = inBuffer.Get(buffer);
	pthread_mutex_unlock(&dataMutex);
	if (!result)
		return 0;

	dataBlock.status = ntohs(*((word*)&buffer[0]));
	dataBlock.mode = ntohs(*((word*)&buffer[2]));
	dataBlock.ready = ntohs(*((word*)&buffer[4]));
	dataBlock.BCstatus = ntohs(*((word*)&buffer[6]));	
	dataBlock.BPstatus = ntohs(*((word*)&buffer[8]));
	dataBlock.channels = ntohs(*((word*)&buffer[10]));
	dataBlock.msgCount = ntohs(*((word*)&buffer[14]));
	dataBlock.breakingPath = ntohl(*((dword*)&buffer[18]));
	dataBlock.odometr = ntohl(*((dword*)&buffer[22]));
	for(int i=0; i<12; i++)
		dataBlock.signals[i] = ntohs(*((word*)&buffer[30+i*2]));

	for(int i=0; i<8; i++)
		dataBlock.msg[i] = ntohs(*((word*)&buffer[60+i*2]));

	for(int i=0; i<dataBlock.msgCount; i++)
	{
		msg = dataBlock.msg[i];
		if (msg > 0 && msg < 140)
		{
			if (msg == 119)
				mainLog.AddMessage((int)msg, stationID, this->number, NULL, dataBlock.breakingPath);
			else
				mainLog.AddMessage((int)msg, stationID, this->number);
		}
		if (msg & 0x8000)
		{
	//		printf("Receive block message\n");
			mainLog.AddMessage((int)msg, stationID, this->number);
			PtSetArg(&args[0], Pt_ARG_COLOR, BLOCK_COLOR, 0);
			PtSetResources(blockLabel, 1, args);
		}
	}
		// RELEASE
		if (dataBlock.status == STATE_STOP && !direction)
		// TEST ONLY
		//if ((data.state.direction == STATE_STOP || data.state.direction == 0) && !direction)
		{
			if (prefDirection != 3)
				direction = prefDirection;
			else
			 	direction = DIRECTION_UP;			
			// add msg dialog
		}
		else if (dataBlock.status == STATE_UP_PE || dataBlock.status == STATE_UP_SE)
			direction = DIRECTION_UP;
		else if (dataBlock.status == STATE_DOWN_PE || dataBlock.status == STATE_DOWN_SE)
			direction = DIRECTION_DOWN;

		UpdateEscalator();
		UpdatePanel();
		UpdateLeds();

	return 1;
}

void metro_escalator::SetState(dword state)
{
	PtArg_t args[1];
	char		buffer[256];
	char		translate[256*MB_LEN_MAX];
	char		message[] =  "Потеряна cвязь с эскалатором %d на станции \"%s\"";
	char		name[128];

	printf("Set state %d\n", id);

	switch (state)
	{
		case 0:
		case 2:
			g_stations[stationID].Name.Get(name, 128);
			sprintf(buffer, message, number, name);
			g_alertQueue.AddAlert(buffer);
//			mainLog.AddMessage(141, parent->GetID(), this->number);
			online = 0;	
			memset(&dataBlock, 0, sizeof(DataBlock));
			UpdateEscalator();
			UpdatePanel();
			break;
		case 1:
			online = 1;
			UpdateEscalator();
			UpdatePanel();
			break;
		default:
			break;
	}
}

int SetUpDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{	
	PtArg_t		arg, arg_other;
	char			**strList = NULL;
	char	name[128];
	const char *directionStr[] = { "подъем"};

	PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	PtSetResources(ABW_SetEscDownBtn, 1, &arg_other);
	PtSetResources(ABW_SetEscReverseBtn, 1, &arg_other);

	if ((g_selectDirection >= 0) && (g_selectDirection < g_escalatorNum))
	{
		g_escalators[g_selectDirection].prefDirection = 1;
		strList=new (char*)[1];
		strList[0] = new char[256];
		if (strList)
		{
			g_stations[g_escalators[g_selectDirection].stationID].Name.Get(name, 128);
			sprintf(strList[0], "%s\t%d\t%s", name, g_escalators[g_selectDirection].GetNumber(), directionStr[0]);
			PtListReplaceItemPos(ABW_EscDirectionList, (const char**)strList, 1, g_selectDirection + 1);	
			delete[] strList;
		}			
	}
	return( Pt_CONTINUE );
}


int SetDownDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t		arg, arg_other;
	char			**strList = NULL;
	char	name[128];
	const char *directionStr[] = { "спуск"};
	int flg;
	
	PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	PtSetResources(ABW_SetEscUpBtn, 1, &arg_other);
	PtSetResources(ABW_SetEscReverseBtn, 1, &arg_other);
	
	if ((g_selectDirection >= 0) && (g_selectDirection < g_escalatorNum))
	{
		g_escalators[g_selectDirection].prefDirection = 2;
		strList=new (char*)[1];
		strList[0] = new char[256];
		if (strList)
		{
			g_stations[g_escalators[g_selectDirection].stationID].Name.Get(name, 128);
			sprintf(strList[0], "%s\t%d\t%s", name, g_escalators[g_selectDirection].GetNumber(), directionStr[0]);
			PtListReplaceItemPos(ABW_EscDirectionList, (const char**)strList, 1, g_selectDirection + 1);
			delete[] strList;
		}
	}


	return( Pt_CONTINUE );
}


int SelectEscalatorDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int		direction;
	PtArg_t		args[3];

	if (cbinfo->reason == Pt_CB_SELECTION && (cbinfo->reason_subtype == Pt_LIST_SELECTION_BROWSE))
	{

		direction = g_escalators[((PtListCallback_t*)cbinfo->cbdata)->item_pos-1].prefDirection;

		switch(direction)
		{
			case 0:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case 1:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case 2:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case 3:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				break;
			default:
				break;
		}		

	}
	PtSetResources(ABW_SetEscUpBtn, 1, &args[0]);
	PtSetResources(ABW_SetEscDownBtn, 1, &args[1]);
	PtSetResources(ABW_SetEscReverseBtn, 1, &args[2]);
	
	g_selectDirection = ((PtListCallback_t*)cbinfo->cbdata)->item_pos-1;
	return( Pt_CONTINUE );
}

void* Run(void* arg)
{
	int		size;
	int		readNum;
	int		sock;
	metro_escalator*		curEscalator;		
	byte		output[512];
	byte		input[512];
	int			reconnect;
	int			count;
	int			result;
	int			counter = 0;
	int			lose = 0;
	int 		time_correct_int=0;	
	
	curEscalator = (metro_escalator*)arg;
	if (!curEscalator)
	{
		SysMessage(ERROR_MSG, "Invalid escalator pointer");
		return 0;
	}

	SysMessage(INFO_MSG, "Start escalator thread (tid=%d)", pthread_self());

	do
	{		
		sock = ConnectToServer(curEscalator->ip);
		
		if (!sock)
		{
			// sleep before reconnect
			sleep(1);
			continue;
		}
		
		reconnect = 0;		
						
		do
		{
			pthread_mutex_lock (&curEscalator->mutex);
			size = curEscalator->outBuffer.Get(output);
			pthread_mutex_unlock (&curEscalator->mutex);
		
			if (!size)
			{
				// timeout before store data
				usleep(1000);
				continue;
			}
			
			// если эскалатор не включен - выход
			if (!curEscalator->enabled)
				return 0;
/*
// Установка времени				
			if (time_correct_int==0) curEscalator->SendTime();
			if (++time_correct_int>TIME_CORR_INTER) time_correct_int=0;
*/			
			readNum = SendBuffer(sock, output, size, input);			
						 			
 			printf ("Readnum %d\n", readNum);
 			
			if (!readNum)
			{
				if (curEscalator->online == 1)
				{
					MsgSendPulse(curEscalator->conID, SIGEV_PULSE_PRIO_INHERIT, 0, curEscalator->GetID());
					reconnect = 1;
				}
			}
			else		
			{			
				if (curEscalator->online <= 0)
				{
					curEscalator->online = 1;
					MsgSendPulse(curEscalator->conID, SIGEV_PULSE_PRIO_INHERIT, 1, curEscalator->GetID());
					// после коннекта - запрос данных
					curEscalator->GetData();
				}
			
				// DEBUG dump modbus packet
//				fprintf(g_debugFile, "Packet [%d] tid %d socket %d:\n", curEscalator->id, pthread_self(), sock);
//				for(int i=0; i<readNum; i++)
//					fprintf(g_debugFile, "%0x ", input[i]);
//				fprintf(g_debugFile, "\n\n"); 
				// DEBUG

				// проверяем команду
				switch(input[1])
				{
					case 1:
					 	// смена статуса
						if (input[3] & 1)
						{							
							if (!curEscalator->GetData())
								SysMessage(ERROR_MSG, "Output buffer overflow");
						}
						break;
					case 4:
						// пришел б~~ок
						printf("In block %d esc %d\n", input[0], curEscalator->GetNumber());
						if ((input[0] == curEscalator->GetNumber()) && input[2] == 76)
						{
							pthread_mutex_lock(&curEscalator->dataMutex);
							result = curEscalator->inBuffer.Add(&input[3], readNum-3);
							pthread_mutex_unlock(&curEscalator->dataMutex);
							if (!result)
								SysMessage(ERROR_MSG, "Input buffer overflow");
							else
								MsgSendPulse(curEscalator->conID, SIGEV_PULSE_PRIO_INHERIT, 3, curEscalator->GetID());								
						}
						break;
					default:				
						break;
				}				
			}			
		} while(!reconnect);
			
		close(sock);

	} while(1);
}

int ConnectToServer(in_addr_t addr)
{
	int		sock;
	struct	sockaddr_in address;
	fd_set	readFD, writeFD, exFD;
	struct timeval timeout;
	int		flags;
	int		result;

	address.sin_addr.s_addr = addr;
	address.sin_port = htons(MODBUS_TCP_PORT);
	address.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (!sock)
	{
		SysMessage(ERROR_MSG, "Fail to create sock");
		return 0;
	}

    const int on = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
     	SysMessage(ERROR_MSG, "Fail to set socket options");
     	
	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
		SysMessage(ERROR_MSG, "Fail to get file status flags");
		close(sock);
		return 0;
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		SysMessage(ERROR_MSG, "Fail to set file status flags");
		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
		SysMessage(ERROR_MSG, "Fail to connect");
		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			SysMessage(ERROR_MSG, "Fail to set file status flags");
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
		SysMessage(ERROR_MSG, "Fail to select socket");
		close(sock);
		return 0;
	}
	else if (!result)
	{
		SysMessage(ERROR_MSG, "Connect timeout expected");
		close(sock);
		return 0;
	}
	else if (IsConnected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			SysMessage(ERROR_MSG, "Fail to set file status flags");
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

int SendBuffer(int sock, byte* output, int size, byte* input)
{
	int		writeNum;
	int		readNum, dataSize;
	struct	timeval timeout;
	int		result;
	fd_set 	allFd, readFd;
	static	int		total = 0, lost = 0;
	uint64_t 	clock1, clock2;
    float 			diffTime;
    
    	send(sock, output, size, 0);    	
    	total++;
    	printf("Send [%d] uid=%d cmd=%d -> ... ", total, output[6], output[7]);
	clock1 = ClockCycles();
	FD_ZERO(&allFd);
	FD_SET(sock, &allFd);

	timeout.tv_sec = RECEIVE_TIMEOUT;
	timeout.tv_usec = 900000;
	
	readFd = allFd;
	result = select(sock + 1, &readFd, NULL, NULL, &timeout);
	if (result < 0)
	{
		SysMessage(ERROR_MSG, "Fail to select on port. %s", strerror(errno));
		return 0;
	}
	else if (result == 0)
	{
		lost++;
		clock2 = ClockCycles();
		diffTime = (clock2-clock1)*1.0/SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		printf("Response - failed (%f sec)\n", diffTime);
		return 0;
	}

	readNum = recv(sock, input, 6, 0);
	
	if (readNum < 0)
		SysMessage(ERROR_MSG, "Fail to recv data");
	else if (readNum == 0)
		return 0;
	if (input[0] || input[1] || input[2] || input[3] || input[4])
	{
		return 0;
	}

	dataSize = input[5];

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	readFd = allFd;
	result = select(sock + 1, &readFd, NULL, NULL, &timeout);
	if (result < 0)
	{
		SysMessage(ERROR_MSG, "Fail to select on port. %s", strerror(errno));
		return 0;
	}
	else if (result == 0)
	{
		return 0;
	}
	
	readNum = recv(sock, input, dataSize, 0);

	if (readNum < 0)
		SysMessage(ERROR_MSG, "Fail to recv data");
	else if (readNum == 0)
		return 0;

	clock2 = ClockCycles();
	diffTime = (clock2-clock1)*1.0/SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	printf("Response - OK (%f sec)\n", diffTime);
	
	return readNum;
}

int metro_escalator::SendCommand(byte cmd)
{

	byte		buffer[14];
	word		crc;
	int			exitCode;
		
	CHECK_ADDRESS(id);
	
//	printf("Send cmd %0x\n", cmd);

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 6;		// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = cmd;
	crc = CRC(&buffer[6], 6);
	buffer[12] = HIBYTE(crc);
	buffer[13] = LOBYTE(crc);

	pthread_mutex_lock (&mutex);
	if (!outBuffer.Add(buffer, 14))
		exitCode = 0;
	else
		exitCode = 1;
	pthread_mutex_unlock (&mutex);

	return exitCode;
}


int metro_escalator::SendTime()
{

	byte		buffer[19];
	word		crc, reg_time_t;
	time_t 	time_now;
	int			exitCode;
	
/*
// Начало: Проверка  - убрать
	tm temp_time;
	
	temp_time.tm_sec=10;
	temp_time.tm_min=11;
	temp_time.tm_hour=18;
	
	temp_time.tm_mday=6;
	temp_time.tm_mon=0;
	temp_time.tm_year=103;
	temp_time.tm_wday=1;
	temp_time.tm_yday=6;
	temp_time.tm_isdst=1;
	
// Конец: Проверка  - убрать
*/
	CHECK_ADDRESS(id);
	
	printf("Send time to %d\n", number);
	
	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 13;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 16;		// команда
	// начиная со второго регистра (нумерация с 0)
	buffer[8] = 0;
	buffer[9] = 1;
	//два регистра
	buffer[10] = 0;
	buffer[11] = 2;
	//счетчик байт
	buffer[12] = 4;
	
	//текущее время
//	time_now=mktime(&temp_time);
	time_now=time(NULL);
	// получение старшего слова
	reg_time_t=(word)((time_now>>1*16) & 0xFFFF);

	buffer[13] = HIBYTE(reg_time_t);
	buffer[14] = LOBYTE(reg_time_t);
	
	// получение младшего слова	
	reg_time_t=(word)((time_now>>0*16) & 0xFFFF);
	buffer[15] = HIBYTE(reg_time_t);
	buffer[16] = LOBYTE(reg_time_t);

	crc = CRC(&buffer[6], 11);
	buffer[17] = HIBYTE(crc);
	buffer[18] = LOBYTE(crc);



	pthread_mutex_lock (&mutex);
	if (!outBuffer.Add(buffer, 19))
		exitCode = 0;
	else
		exitCode = 1;
	pthread_mutex_unlock (&mutex);

	return exitCode;
}


int metro_escalator::CheckStatus()
{
	byte		buffer[14];
	word		crc;
	int			exitCode;
		
	CHECK_ADDRESS(id);

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 6;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 1;		// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 1;
//	crc = CRC(&buffer[6], 6);
//	buffer[12] = HIBYTE(crc);
//	buffer[13] = LOBYTE(crc);
	
	if (pthread_mutex_trylock(&mutex) != EOK)
		return 0;
	if (!outBuffer.Add(buffer, 12))	
	{
		//printf("Overflow %d\n", number);
		exitCode = 0;
	}
	else
		exitCode = 1;
		
	pthread_mutex_unlock (&mutex);

	return exitCode;
}

int metro_escalator::GetData()
{
	byte		buffer[14];
	word		crc;
	int			exitCode;
		
	CHECK_ADDRESS(id);

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// адрес устройств
	buffer[7] = 4;				// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 38;
	crc = CRC(&buffer[6], 6);
	buffer[12] = HIBYTE(crc);
	buffer[13] = LOBYTE(crc);

	pthread_mutex_lock (&mutex);
	if (!outBuffer.Add(buffer, 14))
		exitCode = 0;
	else
		exitCode = 1;
	pthread_mutex_unlock (&mutex);

	return exitCode;
}

int metro_escalator::ConnectToChannel(int channel)
{
	chanID = channel;	
	conID = ConnectAttach(0, 0, channel, _NTO_SIDE_CHANNEL, 0 );
	if (!conID)
	{
		SysMessage(ERROR_MSG, "Fail connect to pulse channel");
		return 0;
	}
	
	return 1;
}

int metro_escalator::DisconnectToChannel()
{
	return ConnectDetach(conID);
}

int metro_escalator::Start()
{
	if (enabled)
	{
		if (pthread_create(&tid, NULL, &Run, this) != EOK)
		{
			SysMessage(ERROR_MSG, "Fail to create escalator's thread");
			return 0;
		}
	}
	
	return 1;
}

int metro_escalator::UpdateLeds()
{
	int 	signal;
	word	temp;
	int		 index = 0;
	PtArg_t		args[1];

	if (panel && (paneled == id))
	{
		if (g_escTypes)
		{
			int typeIndex = type-1;
			for(int j=0; j < g_escTypes[typeIndex].size; j++)
			{
				for(int k=0; k<g_escTypes[typeIndex].blocks[j].size; k++)
				{
					int temp = g_escTypes[typeIndex].blocks[j].signals[k].index / 8;
					int shift = 16 - ((g_escTypes[typeIndex].blocks[j].signals[k].index % 8) + 1)*2;
					int signal = (dataBlock.signals[temp] >> shift) & 0x3;
					switch(signal)
					{	
						case 0:
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[block_green_led], 0);
							break;
						case 1:

							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[block_blue_led], 0);
							break;
						case 2:

							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[block_red_led], 0);
							break;
						case 4:
						default:			

							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, images[block_grey_led], 0);
						break;
					}			

					PtSetResources(g_escTypes[typeIndex].blocks[j].signals[k].led, 1, args);	
				}
			}
		}
	}

	return 1;
}

int PopupControlMenu( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	metro_escalator*	curEscalator;
	
	PtGetResource(widget, Pt_ARG_POINTER, &curEscalator, 0);
		
	if (curEscalator->enabled && (curEscalator->online == 1) && (curEscalator->dataBlock.mode == 3))
	{
		if (curEscalator->blocked)
			WARN_BOX("Нет готовности");
		else
		{
			g_selectedEscalator = curEscalator;		
			ApCreateModule(ABM_EscalatorMenu, widget, cbinfo);
		}
	}

	return( Pt_CONTINUE );
}

int MoveUp( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (g_selectedEscalator && g_selectedEscalator->enabled && g_selectedEscalator->online
		&& (g_selectedEscalator->dataBlock.mode == 3))
	{
		if (g_selectedEscalator->dataBlock.status != 6)
			return Pt_CONTINUE;	
		if (g_selectedEscalator->prefDirection == DIRECTION_DOWN)
		{
			if (ConfirmDirection() == 1)
				g_selectedEscalator->SendCommand(0xE0);
		}
		else if ((g_selectedEscalator->prefDirection == 3) && (g_selectedEscalator->direction == DIRECTION_DOWN))
		{		
			if (ConfirmDirection() == 1)
				g_selectedEscalator->SendCommand(0xE0);
		}
		else		
			g_selectedEscalator->SendCommand(0xE0);
	}

	return( Pt_CONTINUE );
}


int MoveDown( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (g_selectedEscalator && g_selectedEscalator->enabled && g_selectedEscalator->online
		&& (g_selectedEscalator->dataBlock.mode == 3))
	{
		if (g_selectedEscalator->dataBlock.status != 6)
			return Pt_CONTINUE;
		else if (g_selectedEscalator->prefDirection == DIRECTION_UP)
		{
			if (ConfirmDirection() == 1)
				g_selectedEscalator->SendCommand(0xE1);
		}
		else if ((g_selectedEscalator->prefDirection == 3) && (g_selectedEscalator->direction == DIRECTION_UP))
		{		
			if (ConfirmDirection() == 1)
				g_selectedEscalator->SendCommand(0xE1);
		}		
		else
			g_selectedEscalator->SendCommand(0xE1);
	}

	return( Pt_CONTINUE );
}

int MoveStop( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (g_selectedEscalator && g_selectedEscalator->enabled && g_selectedEscalator->online
		&& (g_selectedEscalator->dataBlock.mode == 3))
	{
			g_selectedEscalator->SendCommand(0xE2);
	}
	return( Pt_CONTINUE );
}

int OnSaveDirections( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	SaveDirections("directions.dat");
	return( Pt_CONTINUE );
}


int UnrealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	metro_escalator::paneled = 0;
	metro_escalator::panel = NULL;

	return( Pt_CONTINUE );
}

int RelizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	char	buffer[256];
	char	number[4];
	int		flg, trapped = -1;
	PtWidget_t* panelTabs = ApGetWidgetPtr(widget, ABN_SignalPanel);
	ALL_ESCALATORS
	{
		if (g_escalators[i].id == metro_escalator::paneled)
		{
			if (g_escTypes)
			{
				int typeIndex = g_escalators[i].type-1;
				for(int j=0; j < g_escTypes[typeIndex].size; j++)
				{
					g_escTypes[typeIndex].blocks[j].CreatePanel(panelTabs);
				}
			}
			g_stations[g_escalators[i].stationID].Name.Get(buffer, 256);
			strcat(buffer, " / Эскалатор ");
			itoa(g_escalators[i].number, number, 10);
			strcat(buffer, number);

			PtSetResource(ApGetWidgetPtr(widget, ABN_PanelTitle), Pt_ARG_TEXT_STRING, buffer, 0);
			trapped = i;
		}
	}

	metro_escalator::panel = widget;
	if ((trapped >= 0) && (trapped < g_escalatorNum))
	{
		g_escalators[trapped].	UpdateLeds();
		g_escalators[trapped].	UpdatePanel();
	}

	return( Pt_CONTINUE );
}

int SetupPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	metro_escalator*	escalator;
	char	buffer[256];
	char	number[4];
   	escalator = (metro_escalator*)get_widget_scalar(widget, Pt_ARG_POINTER);
	metro_escalator::paneled = escalator->id;

	if (metro_escalator::panel)
	{
		g_stations[escalator->stationID].Name.Get(buffer, 256);
		strcat(buffer, " / Эскалатор ");
		itoa(escalator->number, number, 10);
		strcat(buffer, number);

		PtSetResource(ApGetWidgetPtr(metro_escalator::panel, ABN_PanelTitle), Pt_ARG_TEXT_STRING, buffer, 0);
		escalator->UpdateLeds();
		escalator->UpdatePanel();
	}

	return( Pt_CONTINUE );
}


int DiscardBlocking( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	metro_escalator*	escalator;
	char	buffer[256];
	char	number[4];
	PtArg_t		args[1];
	
   	escalator = (metro_escalator*)get_widget_scalar(widget, Pt_ARG_POINTER);
   	if (escalator)
   	{

			PtSetArg(&args[0], Pt_ARG_COLOR, NO_BLOCK_COLOR, 0);
			PtSetResources(escalator->blockLabel, 1, args);   		
   	}


	return( Pt_CONTINUE );
}


int SetReverseEscalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg, arg_other;
	char			**strList = NULL;
	char	name[128];
	const char *directionStr[] = { "реверсивный"};
	
	PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	PtSetResources(ABW_SetEscUpBtn, 1, &arg_other);
	PtSetResources(ABW_SetEscDownBtn, 1, &arg_other);
	
	if ((g_selectDirection >= 0) && (g_selectDirection < g_escalatorNum))
	{
		g_escalators[g_selectDirection].prefDirection = 3;
		strList=new (char*)[1];
		strList[0] = new char[256];
		
		if (strList)
		{
			g_stations[g_escalators[g_selectDirection].stationID].Name.Get(name, 128);
			sprintf(strList[0], "%s\t%d\t%s", name, g_escalators[g_selectDirection].GetNumber(), directionStr[0]);

			PtListReplaceItemPos(ABW_EscDirectionList, (const char**)strList, 1, g_selectDirection + 1);
			delete[] strList;
		}
	}
	return (Pt_CONTINUE);
}


int OnChancelDirections( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	LoadDirections ("directions.dat");
	
	return( Pt_CONTINUE );

	}

