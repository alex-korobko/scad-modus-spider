
#ifndef __FILTER_H__
#define __FILTER_H__

class LogFilter
{
public:
	LogFilter();
	virtual ~LogFilter();
	public:
	int		firstEscalatorNum;
	int		lastEscalatorNum;
	time_t	startTime;
	time_t	endTime;	
	int		stationIndex;
	int		allTime;
	struct
	{
		dword	sysMsg: 1;
		dword	alertMsg: 1;
		dword	blockMsg: 1;
		dword	localMsg: 1;
	} messages;
};

#endif
