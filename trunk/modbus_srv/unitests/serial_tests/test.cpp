#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "test.h"
#include "../../global.h"
#include "../../system.h"
#include "../../modbus.h"
#include "../../lbuffer.h"
#include "../../routeinfo.h"

extern int g_debugMode;

int verbose_mode=0;


void usage(char* Prog_Name);

int main (int argc, char* argv[])
{
	FILE*		file;
	char		buffer[BUFF_LEN], *FileName, lptctmp[4];
	unsigned int tmp, tmp1, size_com;
	byte command[BUFF_LEN];
	word crc;
	ModBus_c modbus;
	CmdPool outBuffer;
//Нчало: Это потом удалить!
	byte command_send[BUFF_LEN+2];
	fd_set 			allFd, readFd;
	int				result;
	int				count;
	int				i, j, size;
	struct timeval	silenceTimeout;
	struct timeval	receiveTimeout;
	struct timeval	responseTimeout;
	double			timeout;
	byte			output[BUFF_LEN+2];
	byte			input[BUFF_LEN+2];
	int				noResponse = 0;
	const int responseTime = 1000; // время ожидания ответа от устройства (мсек.)
	const int requestPause = 50000; // мксек
	const int resendTimes = 2;
//Конец: это потмо удалить!	
	
	if (argc<2) {
			usage(argv[0]);
			return 0;
					}; 

	if (argc==3) 
		{
	if (strcmp(argv[2],"-v")==0) 
				{
				verbose_mode=g_debugMode=1;
				} else {
				printf ("Argument %s must be %s\n",argv[2], "-v");
				usage(argv[0]);
				return 0;
				} 
		}; 


	for (tmp=0; tmp<strlen(argv[1]); tmp++)
		{
		printf ("\n[%c]\n",argv[1][tmp]);
		if (!isdigit(argv[1][tmp])) 
						{
						printf ("%s is not integer value\n",argv[1]);
						usage(argv[0]);
						return 0;
							};
		};

	if (atoi(argv[1])<=0 || atoi(argv[1])>255) {
	printf ("%s is not current function number [0-255]\n",argv[1]);
	usage(argv[0]);
	return 0;
	} 

	//инициализация порта
	if (!modbus.InitPort(PORT_NUM, PORT_SPEED))
	{
		printf("Fail to initialize serial port\n");
		return -1;
	}
	
	FileName=(char*) malloc (strlen ("./commands/")+strlen(argv[1]));
	sprintf (FileName, "./commands/%s", argv[1]);

	printf("FileName %s\n",FileName);
	
	file = fopen(FileName, "r");
	if (!file)
	{
		printf( "command file %s is missing", FileName);
		return 0;
	}
	
	memset  (buffer,0, BUFF_LEN);
	memset  (command,0, BUFF_LEN);
	tmp1=size_com=0;

	
	while (fgets	(buffer, BUFF_LEN, file)!=NULL)
	{
		for (tmp=0; tmp<strlen(buffer)-1; ++tmp)
		{	
		if (buffer[tmp]=='#'|| buffer[tmp+1]=='#') break;
		if (buffer[tmp]!=' ' && buffer[tmp]!='\t' && 
			(isdigit(buffer[tmp]) || buffer[tmp]=='a' || buffer[tmp]=='b' || buffer[tmp]=='c' || buffer[tmp]=='d' || buffer[tmp]=='e' || buffer[tmp]=='f') && 
			buffer[tmp+1]!=' ' && buffer[tmp+1]!='\t' && 
			(isdigit(buffer[tmp+1])  || buffer[tmp+1]=='a' || buffer[tmp+1]=='b' || buffer[tmp+1]=='c' || buffer[tmp+1]=='d' || buffer[tmp+1]=='e' || buffer[tmp+1]=='f')) 
						{
						lptctmp[0]=buffer[tmp];
						lptctmp[1]=buffer[tmp+1];
						lptctmp[2]='\0';	
						command[tmp1++]=atoi(lptctmp); 
						};
		if 	 (tmp1==BUFF_LEN-3) break;
		};	
		size_com=tmp1;
		if 	 (tmp1==BUFF_LEN-3) break;			
	}
	
	fclose(file);

	crc  = CRC(&command[0], size_com);
	command[size_com+1] = HIBYTE(crc);
	command[size_com+2] = LOBYTE(crc);
	size_com=size_com+2;



	FileName=(char*) malloc (strlen ("./commands/")+strlen(argv[1]));
	sprintf (FileName, "./commands/%s%s", argv[1],".bin" );

	printf("FileName %s\n",FileName);
	
	file = fopen(FileName, "w+");
	if (file)
	{
	fwrite(&command[2], sizeof(byte),size_com-2,file);	
	fclose(file);	
	};


			
	if (verbose_mode) 
			{
			printf ("Command from file (CRC added):\n");
			tmp1=0;
			for (tmp=0; tmp<size_com; ++tmp)
					{	
					printf ("\t0x%0x",command[tmp]);
					tmp1++;
					if (tmp1>8) {printf("\n"); tmp1=0;}
					};
			printf("\n");
			printf("Send...\n");
			};
		// посылаем команду
		command_send[0]=1;
		command_send[1]=0;				
		command_send[2]=command[0];				
		if (!outBuffer.Put(command_send,size_com+2))  	Log(ERROR, "Output buffer overflow on sending");

//Начало: Это потом удалить !
	memset  (output,0, BUFF_LEN);
	memset  (input,0, BUFF_LEN);
	// устанавливаем наш порт в "очередь" на чтение
	FD_ZERO(&allFd);
	FD_SET(PORT_NUM, &allFd);

	// время передчи одного смвола
	timeout = 10.0/modbus.GetSpeed();

	// минимальный таймаут между посылками
	silenceTimeout.tv_sec = (int)(timeout*3.5);
	silenceTimeout.tv_usec = (int)((timeout*3.5 - silenceTimeout.tv_sec)*1000000);


	// максимальный таймаут приема между двумя соседними байтами в посылке
	receiveTimeout.tv_sec = (int)(timeout*1.5);
	receiveTimeout.tv_usec = (int)((timeout*1.5 - receiveTimeout.tv_sec)*1000000);

	
	// максимальное время ожидания ответа от устройства
	responseTimeout.tv_sec = 1;
	responseTimeout.tv_usec = responseTime*1000;	

	while(1)
	{
		if (noResponse)
		{
			noResponse = 0;
			SendNoResponse(output[0], output[2], output[3]);
		}
		if (verbose_mode)  printf ("Before usleep\n");
		// делаем паузу между запросами
		usleep(requestPause);
		if (verbose_mode)  printf ("After usleep\n");
		output=command_send;
		size=size_com+2;
/*
		// ждем, когда в буффере появятся данные для отправки
		size = outBuffer.Get(output, BUFF_LEN);

		if (!size)
			{
		if (verbose_mode)  printf ("Command buffer is empty ");
		if (!outBuffer.Put(&command_send[0],size_com+2))  	Log(ERROR, "Output buffer overflow on sending");
			continue;
			};
			
		if (verbose_mode)  printf ("Get command [%d bytes]",size);
					
		if (output[1] > resendTimes)
			continue;
*/
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
			result = select(PORT_NUM + 1, &readFd, NULL, NULL, &silenceTimeout);
			if (result < 0)
			{
				Log(ERROR, "fail to select on port. %s", strerror(errno));
				break;
			}
			else if (result > 0)
			{
				// если что-то есть в буффере - очистить
				tcflush(PORT_NUM, TCIOFLUSH);
				Log(DEBUG, "Flush garbage into COM-port");
			}
			
		} while (result > 0);

		if (result < 0)
		{
			noResponse = 1;
			continue;
		}

		// сбрасывае счетчик айт во ходящем бффере
		// и заголовок
		memset(input, 0, 6);

		// посылаем пакет
		write(PORT_NUM, &output[2], size-2);

		if (tcdrain(PORT_NUM)<0) {Log(ERROR,"In tcdrain [%s]",strerror(errno));};
		
//		usleep((int)((size-2)*timeout*1000000));

		usleep((int)(20*timeout*1000000));

		tcflush(PORT_NUM, TCIOFLUSH);
		
		readFd = allFd;
		result = select(PORT_NUM + 1, &readFd, NULL, NULL, &responseTimeout);
		if (result && FD_ISSET(PORT_NUM, &readFd))
			read(PORT_NUM, &input[6], 1);	
		else if (!result)
		{
			if (g_debugMode)
				printf("Not response...\n");

			output[1]++;
			if (g_debugMode)
				printf("Resend packet\n"); 
			modbus.SendBuffer(output, size);			
			continue;
		}
		else
		{
			noResponse = 1;
			Log(ERROR, "Fail to select on port. %s", strerror(errno));
			continue;
		}

		
		// минимально вычитваем три байта
		count = 3;
		for(i=1; i<count; i++)
		{
			readFd = allFd;
			result = select(PORT_NUM + 1, &readFd, NULL, NULL, &receiveTimeout);
			if (result && FD_ISSET(PORT_NUM, &readFd))
			{
				read(PORT_NUM, &input[i+6], 1);
				// если это третий байт, то это число байт в ответе
				if (i == 2)
				{
					// опредеяем размер остатка входящего пакета
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
					printf("Resend packet  - No response\n");
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
			for(j=6; j<count+6; j++)
				printf("0x%0X ", input[j]);
			putchar('\n');
			putchar('\n');
		}

		if ((HIBYTE(crc) == input[i+4]) && (LOBYTE(crc) == input[i+5]))
		{
			// устанавливаем кол-во байт в приемном буффере
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
//Конец: Это потом удалить !


};

void usage(char *Prog_Name)
{
printf ("\nUsage : \n%s command_number [-v]\nwhere :\n\tcommand_number command (from 1 to 255)\n\t-v be verbose (optional)\n",Prog_Name);
};