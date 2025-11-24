#include "global.h"

/*
Local functions
*/
int is_connected(int sock, fd_set* rd, fd_set* wr, fd_set* ex)
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


int connect_to_server(in_addr_t addr)
{
	int		sock;
	struct	sockaddr_in address;
	fd_set	readFD, writeFD, exFD;
	struct timeval timeout;
	int		flags;
	int		result;

	address.sin_addr.s_addr = addr;
	address.sin_port = htons(system_settings::MODBUS_TCP_PORT);
	address.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (!sock)
	{
		g_system_settings.sys_message(system_settings::ERROR_MSG,
															string("Fail to create sock"));
		return 0;
	}

    const int on = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
    		g_system_settings.sys_message(system_settings::ERROR_MSG,
    															string("Fail to set socket options"));
     	
	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to get file status flags"));

		close(sock);
		return 0;
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to set file status flags"));

		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
   		g_system_settings.sys_message(system_settings::ERROR_MSG, 
   															string("Fail to connect"));

		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to set file status flags"));
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
 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Fail to select socket"));

		close(sock);
		return 0;
	}
	else if (!result)
	{
 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Connect timeout expected"));

		close(sock);
		return 0;
	}
	else if (is_connected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
	 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Fail to set file status flags"));

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


int  send_buffer(int sock, 
						metro_device::buffer_data_type output,  
						metro_device::buffer_data_type &input)
{
	int		readNum, dataSize;
	struct	timeval timeout;
	int		result;
	fd_set 	allFd, readFd;
	static	int		total = 0, lost = 0;
	uint64_t 	clock1, clock2;
    float 			diffTime;

//	printf ("input size %d\n", input.size());
    
    	send(sock, &output[0], output.size(), 0);    	
    	total++;
    	printf("Send [%d] uid=%d cmd=%d -> ... ", total, output[6], output[7]);
	clock1 = ClockCycles();
	FD_ZERO(&allFd);
	FD_SET(sock, &allFd);

	timeout.tv_sec = system_settings::RECEIVE_TIMEOUT;
	timeout.tv_usec = 900000;
	
	readFd = allFd;
	result = select(sock + 1, &readFd, NULL, NULL, &timeout);
	if (result < 0)
	{
		
		string mess="Fail to select on port. ";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		
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

	readNum = recv(sock, &input[0], 6, 0);
	
	if (readNum < 0)
	{
		string mess="Fail to recv data";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);

	} else if (readNum == 0)
		return 0;

	printf("\n size %d", readNum);
	
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
		string mess="Fail to select on port. ";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
	
		return 0;
	}
	else if (result == 0)
	{
		return 0;
	}
	
	readNum = recv(sock, &input[0], dataSize, 0);

	if (readNum < 0)
	{
		string mess="Fail to recv data";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
	}
	else if (readNum == 0)
		return 0;

	clock2 = ClockCycles();
	diffTime = (clock2-clock1)*1.0/SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	printf("Response - OK (%f sec)\n", diffTime);
	
	return readNum;
}

int metro_device::create_station_window(int x, int y)
	{
	vector<PtArg_t> args(1);
	vector<char> label_buffer(10);

	ApDBase_t*  dbase=sys_sett_obj->get_widgets_dbase();
    PtSetParentWidget(get_station_widget());

	PtSetArg(&args[0], Pt_ARG_USER_DATA, &this->id_escalator, 0);    
    wnd = ApCreateWidget(dbase, "escalator_wnd", x, y, args.size(), &args[0]);
   	
    if (wnd==NULL)   		
		return 0;

	arrow = ApCreateWidget(dbase, "Arrow", -1, -1, args.size(), &args[0]);

	args.resize(2);
 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "\0", 0);			
	PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::NO_BLOCK_COLOR, 0);			
	block_label = ApCreateWidget(dbase, "blocking", -1, -1, args.size(), &args[0]);

	ltoa(number, &label_buffer[0], 10);
	args.resize(1);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &label_buffer[0], 0);
	num_label = ApCreateWidget(dbase, "escalator_num", -1, -1, args.size(), &args[0]);
			
    if (!arrow && !block_label && !num_label)
		return 0;
  
	this->update_escalator();
	return 1;
}

PtWidget_t*  metro_device::get_station_widget() 
{

metro_stations_container::iterator_metro_stations iter=g_stations.find(id_station);
if(iter!=g_stations.end())  
		{
		return(iter->second.get_wnd());
		}
		else
		{
		string mess="Station widget not found for esc id ";
		vector<char> tmp_buff(10);
		mess+=itoa(id_escalator, &tmp_buff[0], 10);
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);
		return(NULL);
		};
}

metro_device::metro_device( dword  id,
                   int	  id_station,
                   int number,
                   int	 type,
                   int enabled,
                   in_addr_t	ip,
                   int channel
                   )  throw (spider_exception):
                   id(id),
                   id_station(id_station),
                   number(number),
                   type(type),
                   sleepticks(0),
                   enabled(enabled),
                   online(false),
                   last_message_remote_id(0),
                   last_message_remote_time(0),
                   ip(ip),
                   out_buffer(128),
                   in_buffer(128),
                   out_mutex(PTHREAD_RMUTEX_INITIALIZER),
                   in_mutex(PTHREAD_RMUTEX_INITIALIZER),
				   connection_id(0),
                   channel_id(0),
				   pulse(0) {

		connection_id = ConnectAttach(0, 0, channel, _NTO_SIDE_CHANNEL, 0 );
		if (connection_id<0){
			throw spider_excepion("Fail connect to pulse channel");	
		};
		channel_id = channel;	
   };


metro_device::~metro_device(){
		ConnectDetach(connection_id);
		pthread_mutex_destroy(&out_mutex);
		pthread_mutex_destroy(&in_mutex);
};
