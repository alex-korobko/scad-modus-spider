#include "global.h"

int metro_station::create_wnd(PtWidget_t* parent, int x, int y, int width=0)
{
    vector<PtArg_t> args(2);
	PhRect_t	extent;

	width =(2 + 4);
	width+=this->size_escalators_id()*(45+2);

    PtSetParentWidget(parent);
	PfExtentText(&extent, NULL, "Caption08b", name.c_str(), name.size());
	PtSetArg(&args[0], Pt_ARG_WIDTH, __max(extent.lr.x + 15, width), 0);
    	PtSetArg(&args[1], Pt_ARG_TITLE, name.c_str(), 0);
    wnd = ApCreateWidget(g_system_settings.get_widgets_dbase(), "station_wnd", xcoord*x/100 - 10, ycoord*y/100, 2, &args[0]);
    if (!wnd)
		return 0;

	int count=0;
	metro_escalators_container::iterator_metro_escalators esc_iter;
	for (iterator_escalators_id esc_id_iter=escalators_id.begin(); esc_id_iter!=escalators_id.end(); esc_id_iter++, count++)
		{
	
		  esc_iter=g_escalators.find(*esc_id_iter);
		  if (esc_iter!=g_escalators.end()) 
			  {
			  esc_iter->second.create_station_window( count*(45 + 2) + 3, 3);
			  };
	
		};

    	if (PtRealizeWidget(wnd)!=0)
    	{
          		return 0;
 	}
     
    return 1;
}

