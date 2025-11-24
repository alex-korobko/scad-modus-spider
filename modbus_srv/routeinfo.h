#ifndef __ROUTEINFO_H__
#define __ROUTEINFO_H__

#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>



class RouteInfo
{
public:
	RouteInfo ();
	in_addr					gate1, gate2;
	in_addr_t 				master;
	int		Load(const char*	fileName);
};

RouteInfo::RouteInfo ()
{
	gate1.s_addr=0;
	gate2.s_addr=0;
	master=0;
};


int RouteInfo::Load(const char* fileName)
{
	FILE*		file;
	char		buffer[20];
	
	file = fopen(fileName, "rt");
	if (!file)
	{
		Log(ERROR, "fail to open routing info file '%s'", fileName);
		return 0;
	}
		
	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		master = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}

	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		gate1.s_addr = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}

	if (fgets	(buffer, 20, file))
	{
		buffer[strlen(buffer) - 1] = 0;
		gate2.s_addr = inet_addr(buffer);
	}
	else
	{
		fclose(file);
		return 0;
	}
	
	fclose(file);
	return 1;
}



#endif