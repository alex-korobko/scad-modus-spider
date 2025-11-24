#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "contain_msg_types.h"
#include "contain_msg_dict.h"
#include "devices_types.h"

	/*
		begin: adding callback to unrealize event
	*/
	
	int unrealized_led( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
		device_signal  *signal_pointer=NULL;
		
		PtGetResource(widget, Pt_ARG_POINTER, signal_pointer, 0);
		if (signal_pointer!=NULL)
		 {
		 	signal_pointer->set_led(NULL);
		 };
		return( Pt_CONTINUE );
	};

	/*
		end: adding callback to unrealize event
	*/


	/*
		begin: adding callback to unrealize event
	*/
	int unrealized_panel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
		 device_block *block_pointer=NULL;
		
		PtGetResource(widget, Pt_ARG_POINTER, block_pointer, 0);
		if (block_pointer!=NULL)
		 {
		 	block_pointer->set_panel(NULL);
		 };
		return( Pt_CONTINUE );
	};
	/*
		end: adding callback to unrealize event
	*/


int device_signal::create_led(system_settings *sys_sett_obj, PtWidget_t* parent, int column, int row)
{
	vector<PtArg_t> args(7);
	vector<PtCallback_t> callbacks(1);
	PtCallback_t tmp_callback;

	this->set_led(NULL);	
 	
	tmp_callback.event_f=unrealized_led;
	tmp_callback.data=NULL;
	
	callbacks.push_back(tmp_callback);

	
	PtSetParentWidget(parent);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, name.c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
	PtSetArg(&args[2], Pt_ARG_TEXT_IMAGE_SPACING,	 5, 0);
	PtSetArg(&args[3], Pt_ARG_HELP_TOPIC, hint.c_str(), 0);
	PtSetArg(&args[4], Pt_ARG_EFLAGS,	Pt_INTERNAL_HELP, 0);
	PtSetArg(&args[5], Pt_ARG_POINTER,	this, 0);
	PtSetArg(&args[6],  Pt_CB_UNREALIZED,	&callbacks[0], callbacks.size());
		
	led = ApCreateWidget(sys_sett_obj->get_widgets_dbase(), "block_grey_led", 5+65*column, 4+24*row, args.size(), &args[0]);

		if (led!=NULL)
			{	
				return 1;
			} else {
				return 0;
			};
	return 0;
}
	
void metro_device_type::led_create::operator() (int  signal_id)
{
	iterator_signals tmp_signal=	signals_of_type->find(signal_id);
	if (tmp_signal!=signals_of_type->end()) return;
			
	tmp_signal->second.create_led(sys_set_obj, parent, row,column);
	if (++row>=10) 
		{
			column++;	
			row = 0;
		};
}

void metro_device_type::panel_create::operator() (pair_blocks  block_pair) 
{
	vector<PtArg_t> args(4);
	vector<PtCallback_t> callbacks;
	PtCallback_t tmp_callback;
	PtWidget_t* panel;

	block_pair.second.set_panel(NULL);

	tmp_callback.event_f=unrealized_panel;
	tmp_callback.data=NULL;

	callbacks.push_back(tmp_callback);

	PtSetArg(&args[0], Pt_ARG_TITLE, block_pair.second.get_name().c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_FILL_COLOR, 0x00BBC1D9, 0);
	PtSetArg(&args[2], Pt_ARG_OUTLINE_COLOR, 0x00A8A9D8, 0);
	PtSetArg(&args[3],  Pt_CB_UNREALIZED,	&callbacks[0], callbacks.size());
	
	panel = PtCreateWidget(PtPane, parent, args.size(), &args[0]);

	if (panel!=NULL) 
		{
			led_create  led_creater(panel, sys_set_obj, signals_of_type);
			for_each(block_pair.second.signals_id_begin(), block_pair.second.signals_id_end(), led_creater);
			block_pair.second.set_panel(panel);
		};
};


bool metro_device_type::create_panels(PtWidget_t* parent,
														system_settings *sys_set_obj)
{
	if (parent!=NULL) 
		{
		panel_create  panel_creater(parent, sys_set_obj, &type_signals);
		for_each(this->type_blocks.begin(), this->type_blocks.end(), panel_creater);
		return true;
		} else return false;

};


