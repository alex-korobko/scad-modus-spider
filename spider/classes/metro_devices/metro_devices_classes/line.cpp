using namespace std;

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <sys/neutrino.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "station.h"
#include "metro_stations_container.h"
#include "line.h"

void metro_line::create_widget(PtWidget_t* parent) throw (spider_exception){
 metro_stations_container *stations=metro_stations_container::get_instance();
 metro_stations_container::iterator stations_iter;
 int station_id;
 vector<PtArg_t> args;
 unsigned short *station_widget_width, *station_widget_height;
 PtWidget_t  *station_widget;
 PtWidget_t *line_widget;
 PhPoint_t station_position;
 vector<PhPoint_t> line_points(0);
 vector<PhPoint_t> current_line_points(2);

 if (!stations_ids.empty()) {
   stations_ids_interator stations_ids_iter=stations_ids.begin();
   while(stations_ids_iter!=stations_ids.end()) {
           station_id=*stations_ids_iter;
             stations_iter=stations->find(station_id);
             if (stations_iter==stations->end()) {
                    ostringstream message;
                    message<<"metro_line::create_widget: Not found station with id "<<
                                      station_id<<" for line with id "<<id;
                    throw spider_exception(message.str());
               }; //if (stations_iter==stations->end())
           station_position.x=stations_iter->second.get_x_coord();
           station_position.y=stations_iter->second.get_y_coord();

          stations_iter->second.create_wnd(parent);
          station_widget=stations_iter->second.get_station_window();
          if (station_widget==NULL) {
                 ostringstream exception_description;
                 exception_description<<"metro_line::create_widget: Pointer to widget of station with id "
                                                 <<station_id<<" is NULL";
                 throw spider_exception(exception_description.str());
             }; //if (station_widget==NULL)

           args.clear();
           args.resize(2);
           PtSetArg(&args[0], Pt_ARG_WIDTH, &station_widget_width, 0);
           PtSetArg(&args[1], Pt_ARG_HEIGHT, &station_widget_height, 0);
           PtGetResources(station_widget, args.size(), &args[0]);

           station_position.x=station_position.x+*station_widget_width/2;
           station_position.y=station_position.y+*station_widget_height/2;

           line_points.push_back(station_position);
           stations_ids_iter++;
    }; // while(stations_ids_iter!=stations_ids.end()) 

     args.clear();
     args.resize(5);
     PtSetArg(&args[1], Pt_ARG_COLOR, color, 0);
     PtSetArg(&args[2], Pt_ARG_LINE_WIDTH, LINE_WIDTH_IN_PIXELS, 0);
     PtSetArg(&args[3], Pt_ARG_LINE_JOIN, Pg_ROUND_JOIN, 0);
     PtSetArg(&args[4], Pt_ARG_LINE_CAP, Pg_ROUND_CAP, 0);

     for (vector<PhPoint_t>::size_type i=0;i<line_points.size()-1; i++) {
     current_line_points[0]=line_points[i];
     current_line_points[1]=line_points[i+1];
     PtSetArg(&args[0], Pt_ARG_POINTS, &current_line_points[0], current_line_points.size());
	 line_widget=PtCreateWidget( 
                PtLine,
                parent,
                args.size(),
                &args[0]);

    if (line_widget==NULL) {
               throw spider_exception("metro_line::create_widget: Can`t create line widget");
          } else {
              PtWidgetToBack(line_widget);
          };
      };//  for (vector<PhPoint_t>::size_type 
   }; //if (!stations_ids.empty())
}; 


