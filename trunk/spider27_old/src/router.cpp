#include "global.h"

typedef struct {
	struct rt_msghdr		header;
	sockaddr					dest;
	sockaddr					netmask;
	sockaddr					gateway;
} RouteMsg;


static int is_connected(int sock, fd_set* rd, fd_set* wr, fd_set* ex)
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

int ChangeRoute(in_addr dest, in_addr gateway, int flags, int cmd)
{
	int sock;
  	struct ortentry routeEntry;
  	struct sockaddr_in destAddr, gateAddr;

  	memset(&routeEntry, 0, sizeof(routeEntry));

  	memset(&destAddr, 0, sizeof(destAddr));
  	destAddr.sin_family = AF_INET;
  	destAddr.sin_len = sizeof (struct sockaddr_in);
  	destAddr.sin_addr = dest;

  	memset(&gateAddr, 0, sizeof(gateAddr));
  	gateAddr.sin_family = AF_INET;
  	gateAddr.sin_len = sizeof (struct sockaddr_in);
  	gateAddr.sin_addr = gateway;

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

	if (cmd == router::ADD_ROUTE)
	{
  		if (ioctl(sock, SIOCADDRT, &routeEntry) < 0)
    		{
	   		close (sock);
			perror("Error: ");
			return 0;
    		}
    	}
    	else if (cmd == router::DELETE_ROUTE)
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
		printf( "fail to create sock [%s]", strerror(errno));
		return 0;
	}

	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
		printf ("fail to get file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		printf("fail to set file status flags [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
		printf("fail to connect [%s]", strerror(errno));
		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			printf("fail to set file status flags [%s]", strerror(errno));
			close(sock);
			return 0;
		}
		
		return sock;
	}

	FD_ZERO(&readFD);
	FD_SET(sock, &readFD);
	writeFD = readFD;
	exFD = readFD;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	result = select(sock + 1, &readFD, &writeFD, &exFD, &timeout);
	if (result < 0)
	{
		printf("fail to select socket [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (!result)
	{
		printf("connect timeout expected [%s]", strerror(errno));
		close(sock);
		return 0;
	}
	else if (is_connected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
			printf("fail to set file status flags [%s]", strerror(errno));
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


bool Receive(int sock, vector<byte> buffer)
{
	int  size=buffer.size(), readed_now=0;
	
	while (size > 0)
	{
    		readed_now = recv(sock, &buffer[0], 1, 0);
    		if (readed_now < 0)
    		{
        		if (errno == EINTR)
            		continue;
        		printf ("fail to receive data [%s]", strerror(errno));
        		return false;
    		}
    		if (readed_now == 0)
    		{
        		printf ("fail to receive data [%s]", strerror(errno));
        		return false;
    		}
    		size -= readed_now;
	}

	return true;
}


void* Routing(void* arg)
{
	router*		router_ptr;
	
	if (!arg)
		return 0;
		
	router_ptr = (router*)arg;	
	
	while(1)
	{
		router_ptr->loop();
		sleep(10);
	}
}


net::net() : ip(0), curr_gateway_id(0), broken(0), skip_count(0)
{
}

router::~router()
{
	if (pthread_cancel(routerTID) != EOK)
			g_system_settings.sys_message(system_settings::ERROR_MSG, 
																string("can't terminate thread"));
}

bool router::get_net_parameters()
{
enum {ID=0, IP, GATEWAY_ID, ENTRIES_COUNT};
string entry_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
in_addr_t net_ip=INADDR_NONE;
int gateway_id=-1, net_id=-1;

entries_names[ID]="id";
entries_names[IP]="ip";
entries_names[GATEWAY_ID]="gateway_id";

entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(!entry_name.empty())
{
	if (entry_name.compare(entries_names[ID])==0){
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (nets.find(temp_int)==nets.end())
					{
						net_id=temp_int;
					} else 	{
						string message("Net with id  ");
						message+=&temp_str[0];
						message+=" already exist in router nets";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong net id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[IP])==0) 
	{
		net_ip=inet_addr(&temp_str[0]);
		if (net_ip==INADDR_NONE) 
			{
						string message("Wrong net ip  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[GATEWAY_ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (gateways.find(temp_int)!=gateways.end())
					{
						gateway_id=temp_int;
					} else 	{
						string message("Gateway with id  ");
						message+=&temp_str[0];
						message+=" not found in routing gateways";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};

			} else {
						string message("Wrong gateway id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else {
		string message("Unrecognized routing config entry  ");
		message+=entry_name;
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

		return (false);
	};
	
entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
};


if (net_id>0 &&
	gateway_id>0 &&
	net_ip!=INADDR_NONE
	) 
	{
	router_nets::value_type tmp_pair (net_id,
														net 
																(
																net_id,
																net_ip,
																gateway_id,
																router::NO_BROKEN
																)
														);

	if(gateways.find(gateway_id)!=gateways.end())
		{
		
			in_addr net_addr, gate_addr;
			net_addr.s_addr = net_ip;
			gate_addr.s_addr=gateways.find(gateway_id)->second.get_ip();
			ChangeRoute(net_addr, gate_addr, 0, router::ADD_ROUTE);
			nets.insert(tmp_pair);	
		} else { // it`s impossible, must be checked 
				g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found gateway for net !"));		

		};

	} else {
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for net"));		
		return(false);
	};

 return(true);
}

bool router::get_gateway_parameters()
{
enum {ID=0, IP, NAME, ENTRIES_COUNT};
string entry_name, gateway_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
in_addr_t gateway_ip=INADDR_NONE;
int gateway_id=-1;

entries_names[ID]="id";
entries_names[IP]="ip";
entries_names[NAME]="name";

entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(!entry_name.empty())
{
	if (entry_name.compare(entries_names[ID])==0){
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (gateways.find(temp_int)==gateways.end())
					{
						gateway_id=temp_int;
					} else 	{
						string message("Gateway with id  ");
						message+=&temp_str[0];
						message+=" already exist in router gateways";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong gateway id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[IP])==0) 
	{
		gateway_ip=inet_addr(&temp_str[0]);
		if (gateway_ip==INADDR_NONE) {
						string message("Wrong gateway ip  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[NAME])==0) 
	{
		gateway_name=&temp_str[0];
	} else {
		string message("Unrecognized routing config entry  ");
		message+=entry_name;
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

		return (false);
	};
	
entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
};


if (gateway_id>0 &&
	gateway_ip!=INADDR_NONE &&
	!gateway_name.empty()
	)
	{
	router_gateways::value_type tmp_pair (gateway_id,
														gateway 
																(
																gateway_id,
																gateway_ip,
																gateway_name
																)
														);
		
	gateways.insert(tmp_pair);	
	
	} else {
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for gateway"));		
		return(false);
	};

 return(true);
}



bool router::load_routing(const string file_name)
{
 	enum {GATEWAY=0, NET, ENTRIES_COUNT};

	string	section_name;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[GATEWAY]="gateway";
	sections_names[NET]="net";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	};
	section_name=PxConfigNextSection();
	while (!section_name.empty()) 
	{
			if (section_name.compare(sections_names[GATEWAY])==0) 
			{
				if (! get_gateway_parameters ()) {return(false);};
			} else if (section_name.compare(sections_names[NET])==0) {
				if (! get_net_parameters ()) {return(false);};
			} else {
				string message("Unrecognized routing config section ");
				message+=section_name.c_str();
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);
				return 0;
			};
			
			section_name=PxConfigNextSection();
	};	


	if  (PxConfigClose()) 
	{return (true);}
	else
	{
		string message("Can`t close config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return (false);
	};
	
	return (true);

};


int router::start()
{
	if (pthread_create(&routerTID, NULL, &Routing, this) != EOK)
	{
	g_system_settings.sys_message(system_settings::ERROR_MSG, 
														string("Can't create thread"));

		return 0;
	}
	
	return 1;         
}


void router::loop()
{
	int	 sock;

	if (gateways.empty() ||
		nets.empty())
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, 
														string("Not looped router - gateways or nets empty"));

			return ;	
		}; //	if (gateways.em

	iterator_nets iter_nets=nets.begin();
	
	while(iter_nets!=nets.end())
		{
		if (iter_nets->second.get_broken()==router::BROKEN &&
			iter_nets->second.get_skip_count()<=router::SKIP_TEST_COUNT)
			{
				iter_nets->second.set_skip_count(iter_nets->second.get_skip_count()+1);
				iter_nets++;	
				continue;

			} else if (iter_nets->second.get_broken()==router::BROKEN &&
			iter_nets->second.get_skip_count()>=router::SKIP_TEST_COUNT) {
			iter_nets->second.set_skip_count(0);

			}; //	if (iter_nets

		in_addr_t testing_host_addr = (3 << 24) | iter_nets->second.get_ip();		

		sleep(2);
				
		if (!(sock = ConnectToServer(testing_host_addr, router::ECHO_PORT)))
			{

				in_addr routing_network, routing_gate;
				routing_network.s_addr=iter_nets->second.get_ip();

			// getting next gate  - finding current gate and increment it.
			// if that last gate  - getting first gate
				iterator_gateways iter_gates=gateways.find(iter_nets->second.get_curr_gateway_id());
				if (iter_gates!=gateways.end())
					{
						routing_gate.s_addr=iter_gates->second.get_ip();
						ChangeRoute(routing_network, routing_gate, 0, router::DELETE_ROUTE);
						if (++iter_gates==gateways.end()) 
							{
								iter_gates=gateways.begin();
							};		
					} else {
						iter_gates=gateways.begin();		
					};

					routing_gate.s_addr=iter_gates->second.get_ip();						
					ChangeRoute(routing_network, routing_gate, 0, router::ADD_ROUTE);

					iter_nets->second.set_curr_gateway_id(iter_gates->second.get_id());

					iter_nets++;				
					continue;
			}; //	if (!(sock =

		send(sock, "REQUEST", 7, 0);
		vector<byte>	buffer(7);
		if (!Receive(sock, buffer)) 
			{
				iter_nets->second.set_broken(router::BROKEN);
			}  else {
				iter_nets->second.set_broken(router::NO_BROKEN);
			};
	
		close(sock);

		iter_nets++;
		}; //	while(iter_nets!
}
