#include "global.h"

void  alerts_container::push_back(alert_record new_alert_record) 
{
	PtArg_t		arg;
	vector<char> tmp_str;
	tmp_str.resize(20);
	PtMultiTextAttributes_t	*attributes=PtMultiTextCreateAttributes(NULL);
	
	
	PtMultiTextModifyText(ABW_AlertMsg, 
										0,
										0,
										-1,
										new_alert_record.get_string().c_str(),
										new_alert_record.get_string().size(),
										attributes,
										Pt_MT_TEXT_COLOR);

	if (alerts_stack.empty()) // will adding first alert
	{
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 
	}

	alerts_stack.push_back(new_alert_record);

	itoa(alerts_stack.size(), &tmp_str[0], 10);
	
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
	PtSetResources(ABW_AlertCounter, 1, &arg);
	
	time_t tmp_time=new_alert_record.get_time();	
	
	strftime(&tmp_str[0], tmp_str.size(), "%H:%M:%S", localtime (&tmp_time) );
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
	PtSetResources(ABW_AlertTime, 1, &arg);
};		


void  alerts_container::pop_back() 
{
	PtArg_t		arg;
	PtMultiTextAttributes_t	*attributes=PtMultiTextCreateAttributes(NULL);

	alerts_stack.pop_back();

	if (!alerts_stack.empty())
	{					
		vector<char> tmp_str;
		tmp_str.resize(20);

		PtMultiTextModifyText(ABW_AlertMsg, 
											0,
											0,
											-1,
											alerts_stack.rbegin()->get_string().c_str(),
											alerts_stack.rbegin()->get_string().size(),
											attributes,
											Pt_MT_TEXT_COLOR);

		itoa(alerts_stack.size(), &tmp_str[0], 10);
	
		PtSetArg(&arg, Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
		PtSetResources(ABW_AlertCounter, 1, &arg);

		time_t tmp_time=alerts_stack.rbegin()->get_time();		
		strftime(&tmp_str[0], tmp_str.size(), "%H:%M:%S", localtime(&tmp_time) );
		PtSetArg(&arg, Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
		PtSetResources(ABW_AlertTime, 1, &arg);

	} else {
		PtMultiTextModifyText(ABW_AlertMsg, 
											0,
											0,
											-1,
											"",
											0,
											attributes,
											Pt_MT_TEXT_COLOR);

		PtSetArg(&arg, Pt_ARG_TEXT_STRING, "0", 0);
		PtSetResources(ABW_AlertCounter, 1, &arg);
		
		PtSetArg(&arg, Pt_ARG_TEXT_STRING, "", 0);
		PtSetResources(ABW_AlertTime, 1, &arg);	
		
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 
	};

};

