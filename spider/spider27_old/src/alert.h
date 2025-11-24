#ifndef __ALERT_H__
#define __ALERT_H__

/*****************************************************************************
Container class for alerts - 
wrapper of STL container  vector with stack emulation
*****************************************************************************/

class alert_record
{
	private:
		time_t	time;
		string	message;
		
		alert_record();	
	public :
		alert_record (time_t new_time, string new_message): time(new_time), message(new_message) {};
		
		time_t get_time() {return (time);}
		string get_string() {return(message);}
};

class alerts_container
{
	protected:
		typedef  vector<alert_record> container_alerts;
		container_alerts alerts_stack;		
	public:
		container_alerts::size_type size() {return(alerts_stack.size());}
		bool empty() {return(alerts_stack.empty());}
		
		void  push_back(alert_record new_alert_record);		
		void  pop_back();
};

#endif