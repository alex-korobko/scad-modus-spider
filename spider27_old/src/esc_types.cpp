#include "global.h"

	/*
		begin: adding callback to unrealize event
	*/
	
	int unrealized_led( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
		escalator_signal  *signal_pointer=NULL;
		
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
		 escalator_block *block_pointer=NULL;
		
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


int escalator_signal::create_led(PtWidget_t* parent, int column, int row)
{
	vector<PtArg_t> args(7);
	vector<PtCallback_t> callbacks(1);
	PtCallback_t tmp_callback;
	
	if (led!=NULL) 
		{
				PtSetArg(&args[0], 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( led,
					                    1,
                    						&args[0]);
 		}
 	
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
		
	led = ApCreateWidget(g_system_settings.get_widgets_dbase(), "block_grey_led", 5+65*column, 4+24*row, args.size(), &args[0]);

		if (led!=NULL)
			{	
				return 1;
			} else {
				return 0;
			};
	return 0;
}


int escalator_block::create_panel(PtWidget_t* parent)
{
	vector<PtArg_t> args(5);
	vector<PtCallback_t> callbacks;
	PtCallback_t tmp_callback;
	
	if (panel!=NULL) 
		{
				PtSetArg(&args[0], 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( panel,
					                    1,
                    						&args[0]);
 		}

	tmp_callback.event_f=unrealized_panel;
	tmp_callback.data=NULL;

	callbacks.push_back(tmp_callback);


	PtSetArg(&args[0], Pt_ARG_TITLE, name.c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_FILL_COLOR, 0x00BBC1D9, 0);
	PtSetArg(&args[2], Pt_ARG_OUTLINE_COLOR, 0x00A8A9D8, 0);
	PtSetArg(&args[3], Pt_ARG_POINTER,	this, 0);
	PtSetArg(&args[4],  Pt_CB_UNREALIZED,	&callbacks[0], callbacks.size());
	
	panel = PtCreateWidget(PtPane, parent, args.size(), &args[0]);

	if (panel!=NULL) 
	{
		
		escalator_block::led_create  created_panel(panel);
		for_each(this->signals_begin(), this->signals_end(), created_panel);

		PtRealizeWidget(panel);

		return (1);
	} else {
		return(0);
	};
	return(0);
}

