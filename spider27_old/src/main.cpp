/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01A */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

#define DEFINE_GLOBALS

/* Local headers */
#include <mqueue.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/neutrino.h>
#include "global.h"
#include "abimport.h"
#include "proto.h"
#include "sound.h"

#include "log.h"
#include "alert.h"
#include "dictionary.h"

FILE* cfg_file;
PtWidget_t *escal[256];
int escal_count = 0;
PtWidget_t* test_led;

char		g_leftGate[] = "192.168.0.1";
char		g_rightGate[] = "192.168.0.2";

pthread_t		pingTID, timerTID;

enum {lineRead = 1, stationRead, escalatorRead};

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */
	
void* PingThread(void* arg)
{
SysMessage(INFO_MSG,"In PingThread");				 	
	while(1)
	{

		ALL_ESCALATORS
	 	{
	 		if ((g_escalators[i].online ==1) && g_escalators[i].enabled)
	 		{
	 			if (g_escalators[i].sleepticks >= 1)
	 			{
	 				g_escalators[i].CheckStatus();	
	 				g_escalators[i].sleepticks = 0;
	 			}
	 			else
	 				g_escalators[i].sleepticks++;
	 		}
	 		else if (g_escalators[i].enabled)
	 		{
				if (g_escalators[i].sleepticks >= 5)
	 			{
	 				g_escalators[i].CheckStatus();	
	 				g_escalators[i].sleepticks = 0;
	 			}
	 			else
	 				g_escalators[i].sleepticks++;	 		
	 		}
		
 		}

 		usleep(PING_TIMEOUT);
 	}
}

void* TimerThread(void *arg)
{
	int             recvID;
	struct _pulse   pulse;
	timer_t         timerID;
	struct sigevent event;
	struct itimerspec timer;
	int             conID;
	int             timeoutChannel;
	if ((timeoutChannel = ChannelCreate(0)) == -1)
	{
		SysMessage(ERROR_MSG,"Can't create timeout channel [%s]", strerror(errno));
		return 0;
	}
	
	
	conID = ConnectAttach(0, 0, timeoutChannel, 0, 0);
	if (conID == -1) {
		SysMessage(ERROR_MSG,"Attach channel [%s]", strerror(errno));
		return 0;
	}
	
	
	SIGEV_PULSE_INIT(&event, conID, SIGEV_PULSE_PRIO_INHERIT, SYSTEM_TIMER, 0);

	if (timer_create(CLOCK_REALTIME, &event, &timerID) == -1)
	{
		SysMessage(ERROR_MSG,"Fail to create system timer [%s]", strerror(errno));
	}

	
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_nsec = 0;
	timer.it_interval.tv_sec = SYSTEM_TICK;
	timer.it_interval.tv_nsec = 0;

	timer_settime(timerID, 0, &timer, NULL);

	while (1)
	{
		recvID = MsgReceive(timeoutChannel, &pulse, sizeof(pulse), NULL);
		
		if (recvID==-1)  SysMessage(ERROR_MSG,"MsgReceive [%s]", strerror(errno));
	
		if (recvID == 0) {
		SysMessage(INFO_MSG,"In  recvID [%d]", recvID);		
			if (pulse.code == SYSTEM_TIMER) 
			{
			mainLog.Rotate();
			}
		}
	}

	return 0;
}

int LoadMap(const char* fileName)
{
	char	buffer[256];
	char	temp[40];
	FILE*	handle;
   	char 	*val_name, *val_data;
    	char  	tokens[] = "=";
	int	   	mode = 0;
	int 	   	intTemp, i;
	int		lineCount = -1, stationCount = -1, escalatorCount = -1;

	handle = fopen(fileName, "rt");
	if (!handle)
		return 0;
		
	while(fgets(buffer, 256, handle))
	{
		if (extract_string(buffer, temp, '[', ']'))
		{
			if (!strcmp("line", temp))
			{
				mode = lineRead;
				lineCount++;
				if (lineCount >= g_lineNum)
				{
					lineCount = -1;
				//	printf("Error line!\n");
					fclose(handle);
					return 0;
				}
			}
			else if (!strcmp("station", temp))
			{
				mode = stationRead;
				stationCount++;
				if (stationCount >= g_stationNum)
				{
					stationCount = -1;
				//	printf("Error station!\n");
					fclose(handle);
					return 0;
				}						
			}
			else if (!strcmp("escalator", temp))
			{
				mode = escalatorRead;
				escalatorCount++;		
				if (escalatorCount >= g_escalatorNum)
				{
					escalatorCount = -1;
			//		printf("Error escalator!\n");
					fclose(handle);
					return 0;
				}						
			}
		}
		else if (buffer[0] != '#')
		{
			val_name = strtok(buffer, tokens);
			val_data = strtok(NULL, tokens);
			if (val_name && val_data)
			{
				if (!strcmp("lineNum", val_name))
				{					
					g_lineNum = atoi(val_data);
					if (g_lineNum)
						g_lines = new metro_line[g_lineNum];
				}
				else if (!strcmp("stationNum", val_name))
				{					
					g_stationNum = atoi(val_data);
					if (g_stationNum)
						g_stations = new metro_station[g_stationNum];
				}
				else if (!strcmp("escalatorNum", val_name))
				{					
					g_escalatorNum = atoi(val_data);
					if (g_escalatorNum)
						g_escalators = new metro_escalator[g_escalatorNum];
				}
				else if (!strcmp("color", val_name))
				{					
					g_lines[lineCount].color = strtoul(val_data, NULL, 16);
				}
				else if (!strcmp("lineID", val_name))
				{					
					intTemp = atoi(val_data);
					g_stations[stationCount].lineID = intTemp;
					if ((intTemp >= 0) && (intTemp < g_lineNum))
						g_lines[intTemp].stationCount++;
					//else
					//	printf("Error\n");
					
				}
				else if (!strcmp("stationID", val_name))
				{					
					g_escalators[escalatorCount].stationID = atoi(val_data);
				}
				else if (!strcmp("x", val_name))
				{					
					g_stations[stationCount].xcoord = atoi(val_data);
				}
				else if (!strcmp("y", val_name))
				{					
					g_stations[stationCount].ycoord = atoi(val_data);
				}
				else if (!strcmp("type", val_name))
				{					
					g_escalators[escalatorCount].type = atoi(val_data);
				}
				else if (!strcmp("id", val_name))
				{					
					switch(mode)
					{
						case lineRead:
							g_lines[lineCount].id = atoi(val_data);
							break;
						case stationRead:
							g_stations[stationCount].id = atoi(val_data);
							break;
						case escalatorRead:
							g_escalators[escalatorCount].id = atoi(val_data);
							break;
						default:
							break;
					}
				}
				else if (!strcmp("num", val_name))
				{					
					g_escalators[escalatorCount].number = atoi(val_data);
				}
				else if (!strcmp("enabled", val_name))
				{					
					g_escalators[escalatorCount].enabled = atoi(val_data);
				}		
				else if (!strcmp("ip", val_name))
				{					
					g_escalators[escalatorCount].ip = inet_addr(val_data);
				}
				else if (!strcmp("direction", val_name))
				{
					int direction = atoi(val_data);
					// Stupid!!
					switch(direction)
					{
						case DIRECTION_UP:
							g_escalators[escalatorCount].prefDirection = DIRECTION_UP;
							break;
						case DIRECTION_DOWN:
							g_escalators[escalatorCount].prefDirection = DIRECTION_DOWN;
							break;
						case DIRECTION_REVERSE:
							g_escalators[escalatorCount].prefDirection = DIRECTION_REVERSE;
							break;
						default:
							break;
					}
				}
				else if (!strcmp("name", val_name))
				{
					char* linefield = strchr(val_data, '\n');
					if (linefield)
						*linefield = 0;
					switch(mode)
					{
						case lineRead:
							g_lines[lineCount].Name.Set(val_data);
							break;
						case stationRead:
							g_stations[stationCount].Name.Set(val_data);
							break;
						default:
							break;
					}
				}
			}
		}
	}

	int*	indexes = new int[g_lineNum];
	for(i=0; i<g_lineNum; i++)
	{
		indexes[i] = 0;
		g_lines[i].points = new PhPoint_t[g_lines[i].stationCount];
		if (!g_lines[i].points)
			return 0;
	}
	
	for(int j=0; j<g_stationNum; j++)
	{
		int t = indexes[g_stations[j].lineID];
		g_lines[g_stations[j].lineID].points[t].x = g_stations[j].xcoord;
		g_lines[g_stations[j].lineID].points[t].y = g_stations[j].ycoord;		
		indexes[g_stations[j].lineID]++;
	}
	
	for(i=0; i<g_escalatorNum; i++)
		g_stations[g_escalators[i].stationID].escalatorNum++;
	
	delete[] indexes;
	fclose(handle);
	
	return 1;
}

int CreateScheme()
{
	PhDim_t		*size;
	int				i, j;
	PtArg_t		args[3];
	PhPoint_t	*temp = NULL;

    PtGetResource(ABW_Scheme, Pt_ARG_DIM, &size, 0);

	for(i=0; i<g_lineNum; i++)
	{
		temp = new PhPoint_t[g_lines[i].stationCount];
		if (!temp)
			return 0;
		
		memcpy(temp, g_lines[i].points, g_lines[i].stationCount*sizeof(PhPoint_t));
		for(j=0; j<g_lines[i].stationCount; j++)
		{
			temp[j].x = temp[j].x*size->w/100;
			temp[j].y = temp[j].y*size->h/100;
		}
		
	 	PtSetArg(&args[0], Pt_ARG_COLOR, g_lines[i].color, 0);
	 	PtSetArg(&args[1], Pt_ARG_POINTS, temp, g_lines[i].stationCount);	 	
	 	PtSetArg(&args[2], Pt_ARG_LINE_WIDTH, 6, 0);
	    g_lines[i].line = PtCreateWidget(PtPolygon, ABW_Scheme, 3, args);
        	delete[] temp;
        	temp = NULL;
        	j= PtRealizeWidget(g_lines[i].line);
	    if (!g_lines[i].line || j== -1)
      		return 0;
	}

	for(i=0; i<g_stationNum; i++)
	{
		PtSetParentWidget(ABW_Scheme);
		if (!g_stations[i].create_wnd(widget_dbase, ABW_Scheme, size->w, size->h, -1, -1))
			return 0;
	}
	return 1;
}

int ResizeScheme( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t		arg;
	PhPoint_t	*temp = NULL;
	PhPoint_t	point;
	PhDim_t		*wndSize;
	int flg;
		
	PtContainerCallback_t	*data = (PtContainerCallback_t*)cbinfo->cbdata;
	for(int i=0; i<g_lineNum; i++)
	{
		if (g_lines[i].line)
		{
			temp = new PhPoint_t[g_lines[i].stationCount];
			if (!temp)
				return 0;
		
			memcpy(temp, g_lines[i].points, g_lines[i].stationCount*sizeof(PhPoint_t));
			for(int j=0; j<g_lines[i].stationCount; j++)
			{
				temp[j].x = temp[j].x*data->new_dim.w/100;
				temp[j].y = temp[j].y*data->new_dim.h/100;
			}
			
		 	PtSetArg(&arg, Pt_ARG_POINTS, temp, g_lines[i].stationCount);	 	
		 	PtSetResources(g_lines[i].line, 1, &arg);
 	        	delete[] temp;
	        	temp = NULL;
	    }
	}
	for(int i=0; i<g_stationNum; i++)
	{
		if (g_stations[i].wnd)
		{		
			PtSetArg(&arg, Pt_ARG_DIM, &wndSize, 0);
			PtGetResources(g_stations[i].wnd, 1, &arg);
			point.x = g_stations[i].xcoord*data->new_dim.w/100 - wndSize->w/2;
			point.y = g_stations[i].ycoord*data->new_dim.h/100 - wndSize->h/2;

	 		PtSetArg(&arg, Pt_ARG_POS, &point, 0);
	 		PtSetResources(g_stations[i].wnd, 1, &arg);		
	 	}
	}
	return( Pt_CONTINUE );
}

PtInputCallbackProcF_t PulseReceiver;
int PulseReceiver(void *data, int rcvid, void *message, size_t mbsize )
{
	struct _pulse* pulse;
	metro_escalator*		escalator;

	pulse = (struct _pulse*)message;	
//	printf("Code %d, value %d\n", pulse->code, pulse->value.sival_int);
//	escalator = g_escalators[pulse->value.sival_int];
	for(int i=0; i<g_escalatorNum; i++)
		if (g_escalators[i].id == pulse->value.sival_int)
			escalator = &g_escalators[i];
	if (!escalator)
	{
		SysMessage(ERROR_MSG, "can't find current escalator");
		return Pt_HALT;
	}

//	printf("Code %d, value %d ESCALATOR %p on %d\n", pulse->code, pulse->value.sival_int, escalator, escalator->online);
	switch(pulse->code)
	{
		case 0:
		case 1:
		case 2:
			escalator->SetState(pulse->code);			
			break;
		case 3:
			escalator->SetData();
			break;
		default:
			break;
	}
	
	PtFlush();
	
	return Pt_HALT;
}

int Initialize( int argc, char *argv[] )
{
	PtInputId_t *InputRes=NULL;
    	/* eliminate 'unreferenced' warnings */
    	argc = argc, argv = argv;
   
    	g_debugFile = fopen("debug.log", "a+");
    	if (!g_debugFile)
    		PtExit(EXIT_FAILURE);

	// загружаем библиотеку виджетов

  	widget_dbase = ApOpenDBase(ABM_interface);	
    
    	SysMessage(INFO_MSG, "Spider: startup");
    
	// файл сообщений для журнала
    	if (!LoadDictionary("messages.dat"))
	{
		ERROR_BOX("Файл сообщений журнала не найден или поврежден");
		PtExit(EXIT_FAILURE);
	}

	// инициализируем звук
/*    	if (!g_sound.Init())
	{
		WARN_BOX("Не инициализирован аудио драйвер");
	}*/
            
	if (!LoadEscalatorLibrary("library.dat"))
	{
		ERROR_BOX("Файл библиотеки эскалаторов не найден или поврежден");
		PtExit(EXIT_FAILURE);
	}

	// загрузка файла схемы
	if (!LoadMap("config.ini"))
    	{
		ERROR_BOX("Файл схем метрополина не найден или поврежден");
		PtExit(EXIT_FAILURE);
    	}
    
    	LoadDirections("directions.dat");

	// инциализация роутера
	// шлюз для "левой" ветки
	g_router.AddLeftLeafGate(g_leftGate);
	// шлюз для "правой" ветки
	g_router.AddRightLeafGate(g_rightGate);
	
	// описание "левой" ветки
	if (!g_router.LoadLeft("leftleaf.cfg"))
    	{
		WARN_BOX("Файл конфигурации 'левой' ветки сети не найден или поврежден");
    	}

	// описание "правой" ветки
	if (!g_router.LoadRight("rightleaf.cfg"))
    	{
		WARN_BOX("Файл конфигурации 'левой' ветки сети не найден или поврежден");
    	}

	// запускаем поток роутера
	g_router.Start();
	
	// канал для пульсов
	g_chanID = PhChannelAttach(0, -1, NULL);
	if (g_chanID < 0)
	{
		ERROR_BOX("Ошибка создания канала IPC");
		PtExit(EXIT_FAILURE);
	}
	else
		SysMessage(INFO_MSG, "Create pulse channel (id=%d)", g_chanID);

	InputRes=PtAppAddInput(NULL, 0, PulseReceiver, NULL);
    	if (InputRes== NULL)
    	{
		ERROR_BOX("Ошибка регистрации обработчика пульсов");
          PtExit(EXIT_FAILURE);
    	}
 
	ALL_ESCALATORS
	{
		g_escalators[i].ConnectToChannel(g_chanID);	
		g_escalators[i].Start();
	}

	// запуск задачи опроса устройств
	if (pthread_create(&pingTID, NULL, &PingThread, NULL) != EOK)
	{
		ERROR_BOX("Ошибка запуска менеджера устройств");
		PtExit(EXIT_FAILURE);
	}
	else
		SysMessage(INFO_MSG, "Start ping thread");

	if (pthread_create(&timerTID, NULL, &TimerThread, NULL) != EOK)
	{
		ERROR_BOX("Ошибка запуска системного таймера");
		PtExit(EXIT_FAILURE);
	}
	else
		SysMessage(INFO_MSG, "Start system timer thread");
		
//	g_sound.Play("startup.wav");
//	g_sound.Play("test.wav");
	
	return( Pt_CONTINUE );
}

int Uninitialize(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
	if (pthread_cancel(pingTID) != EOK)
		SysMessage(ERROR_MSG, "Close ping thread [%s]", strerror(errno));		

	if (pthread_cancel(timerTID) != EOK)
		SysMessage(ERROR_MSG, "Close system timer thread [%s]", strerror(errno));		

	ALL_ESCALATORS
	{
		g_escalators[i].DisconnectToChannel();
	}

	// зыкрываем канал
	ChannelDestroy(g_chanID);
	SysMessage(INFO_MSG, "Close pulse channel");
	
	// сохраняем лог
	mainLog.Save("main.log");

	FreeEscalatorTypes();

	// закрываем библоиотеку виджетов
	ApCloseDBase(widget_dbase);
		
	fclose(g_debugFile);
	return( Pt_CONTINUE );
}

int RealizeMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// создаем схему

	CreateScheme();

	// загружаем журнал
	mainLog.Load("main.log");

	// регистрируем окно для журнала
	mainLog.AttachWnd(ABW_log_wnd);
	
	// добавляем сообщение
	mainLog.AddMessage(143, 0, 0); // Program started
 
    	return( Pt_CONTINUE );	
}

int CloseMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{

	
	mainLog.AddMessage(144, 0, 0); // Program closed

	return( Pt_CONTINUE );
}

int FillEscConfList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
     	 metro_station** stations;
		metro_escalator*		curEscalator;
		char	**strList;
		char	*name;
		const char *directionStr[] = { "не установлено", "подъем", "спуск" ,"реверсивный"};
		int		flg, count = 0;
		char		str[128], str1[128];
	
	strList = new (char*)[g_escalatorNum];	
	for(int i=0; i<g_escalatorNum; i++)
	{		
		strList[i] = new char[256];
		g_stations[g_escalators[i].stationID].Name.Get(str, 128);
		
		sprintf(strList[i], "%s\t%d\t%s", str, g_escalators[i].GetNumber(), directionStr[g_escalators[i].prefDirection]);
	}
	
		PtListAddItems(ABW_EscDirectionList, (const char**)strList, g_escalatorNum, 0);	

	//Уже было закомменчено.
   	//PtSetResource(ABW_EscDirectionList, Pt_ARG_POINTER, strList, 0);
 
 	delete[] strList;  		

	return( Pt_CONTINUE );
}


int OpenArchive( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtWidget_t*	wnd = NULL;

	if (wnd = g_archiveLog.GetWindow())
	{
		PtWindowToFront(wnd);
		return Pt_IGNORE;
	}

	return( Pt_CONTINUE );
}



