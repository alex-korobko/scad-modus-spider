/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

/* Local headers */
#include "ablibs.h"
#include "global.h"
#include "abimport.h"
#include "proto.h"

#include "sound.h"
#include "alert.h"

AlertQueue::AlertQueue()
{
	lastMsgLength = 0;
}

AlertQueue::~AlertQueue()
{
	AlertRecord		*curRecord;
	
	alerts.first();
	while(curRecord = alerts.next())
	{
		delete[] curRecord->msg;
		delete curRecord;
	}	
}

int AlertQueue::AddAlert(const char* msg)
{
	PtMultiTextAttributes_t	attributes;
	AlertRecord		*record;
	int		msgLength;
	PtArg_t		arg;
	char		buffer[20];
	int			counter;
	
	if (!msg)
		return 0;
		
	record = new AlertRecord;
	assert(record);

	msgLength = 	strlen(msg);
	record->time = time(NULL);
	record->msg = new char[msgLength+1];
	assert(record->msg);
	strcpy(record->msg, msg);
	
	alerts.Push(record);
		
	attributes.font = Pt_DEFAULT_FONT;
	attributes.background_color = Pt_DEFAULT_COLOR;
	attributes.text_color = Pt_DEFAULT_COLOR;
/*	switch(type)
	{
		case 0:
			attributes.text_color = PgRGB(255,0,0);
			break;
		case 1:
			attributes.text_color = PgRGB(0,120,0);
			break;
		case 2:
			attributes.text_color = PgRGB(0,0,120);
			break;
		default:
			attributes.text_color = Pt_DEFAULT_COLOR;
			break;			
	}*/
	
	PtMultiTextModifyText(ABW_AlertMsg, 0, lastMsgLength, -1, msg, strlen(msg), &attributes, Pt_MT_TEXT_COLOR);
	lastMsgLength = msgLength;
	
	counter = alerts.GetSize();
	itoa(counter, buffer, 10);
	
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, buffer, 0);
	PtSetResources(ABW_AlertCounter, 1, &arg);
	
	if (counter == 1)
	{
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 
	}
	
	strftime(buffer, 20, "%H:%M:%S", localtime(&record->time));
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, buffer, 0);
	PtSetResources(ABW_AlertTime, 1, &arg);
	
//	g_sound.Play("test.wav");
	
	return 1;
}

int NextAlert( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	AlertRecord		*curRecord;
	PtMultiTextAttributes_t	attributes;
	AlertRecord		*record;
	int		msgLength;
	PtArg_t		arg;
	char		buffer[20];
	int			counter;
	int lastMsgLength;
	
//	g_alertQueue.alerts.last();
	record = g_alertQueue.alerts.Pop();
	if (record)
	{
		lastMsgLength = strlen(record->msg);	
		delete[] record->msg;
		delete record;	
	}
	
	record = g_alertQueue.alerts.last();
	if (record)
	{
	msgLength = 	strlen(record->msg);
			
	attributes.font = Pt_DEFAULT_FONT;
	attributes.background_color = Pt_DEFAULT_COLOR;
	attributes.text_color = Pt_DEFAULT_COLOR;

	PtMultiTextModifyText(ABW_AlertMsg, 0, lastMsgLength, -1, record->msg, strlen(record->msg), &attributes, Pt_MT_TEXT_COLOR);
	counter = g_alertQueue.alerts.GetSize();
	itoa(counter, buffer, 10);
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, buffer, 0);
	PtSetResources(ABW_AlertCounter, 1, &arg);
	
	strftime(buffer, 20, "%H:%M:%S", localtime(&record->time));
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, buffer, 0);
	PtSetResources(ABW_AlertTime, 1, &arg);
	}
	else
	{
	attributes.font = Pt_DEFAULT_FONT;
	attributes.background_color = Pt_DEFAULT_COLOR;
	attributes.text_color = Pt_DEFAULT_COLOR;
	
	PtMultiTextModifyText(ABW_AlertMsg, 0, lastMsgLength, -1, "", 0, &attributes, Pt_MT_TEXT_COLOR);

	PtSetArg(&arg, Pt_ARG_TEXT_STRING, "0", 0);
	PtSetResources(ABW_AlertCounter, 1, &arg);
		
	PtSetArg(&arg, Pt_ARG_TEXT_STRING, "", 0);
	PtSetResources(ABW_AlertTime, 1, &arg);	
	}	
	
	return( Pt_CONTINUE );
}


int
ActivateAlertBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg;
	int 	counter = g_alertQueue.alerts.GetSize();
	
		if (!counter)
		{
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 
		}
		
	return( Pt_CONTINUE );

	}

