#ifndef __ALERT_H__
#define __ALERT_H__

#include "list.h"

class AlertRecord
{
	public:
		time_t	time;
		char*		msg;
};

class AlertQueue
{
	protected:
		
	public:
		List<AlertRecord> alerts;
		int		lastMsgLength;
	
		AlertQueue();
		virtual ~AlertQueue();
		int AddAlert(const char* msg);		
};

#endif