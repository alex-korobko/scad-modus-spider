using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <pthread.h>
#include <process.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <hash_map>
#include <sstream>

#include "emulation.h"
#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"
#include "udku_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"

#include "devices_types.h"


static PtWidget_t * inflate_data_block_balloon( PtWidget_t *window,
                             PtWidget_t *widget,
                             int position,
                             char *text,
                             char *font,
                             PgColor_t fill_color,
                             PgColor_t text_color ) {
	device_data_unit *data_unit=NULL;
	PtGetResource(widget, Pt_ARG_POINTER, &data_unit, 0);
	if (data_unit!=NULL){
		 string data_unit_hint=data_unit->get_hint();
		if (data_unit_hint.size()>0) 
         return PtInflateBalloon( window,
                             widget,
                             position,
                             data_unit_hint.c_str(),
                             font,
                             fill_color,
                             text_color );
     }; //if (pointer_to_void!=NULL)
         return NULL;
};

PhImage_t* device_signal::signal_image(data_block *data_blk,
                                                             system_settings_spider *sys_sett,
                                                             int signal_index){
data_block::signal_data_type signal_value;
try {
    signal_value=data_blk->get_signal_value(signal_index);
} catch (spider_exception spr_exc) {
    ostringstream exception_description;
    exception_description<<
              "In device_signal::signal_image(...) "<<
              spr_exc.get_description()<<
              " signal index "<<signal_index;
  sys_sett->sys_message(system_settings::ERROR_MSG,
                                 exception_description.str());
  return sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED);                                 
};

if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR) {

	if(escalator_data_block::SIGNAL_VALUE_GREEN==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED);
	if(escalator_data_block::SIGNAL_VALUE_BLUE==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_BLUE_LED);
	if(escalator_data_block::SIGNAL_VALUE_RED==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_RED_LED);
    return sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED);

     }; //if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR

if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_SHAVR) {

	if(shavr_data_block::SIGNAL_VALUE_GREEN==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED);
	if(shavr_data_block::SIGNAL_VALUE_BLUE==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_BLUE_LED);
    return sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED);

    }; //if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_SHAVR

if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_UDKU) {

	if(udku_data_block::SIGNAL_VALUE_GREEN==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED);
	if(udku_data_block::SIGNAL_VALUE_RED==signal_value)
                  return sys_sett->get_image(system_settings_spider::BLOCK_RED_LED);
    return sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED);

    }; //if(data_blk->get_data_block_type_description()==data_block::DATA_BLOCK_UDKU


//impossible
return sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED);
};

bool device_parameter::value_is_norma(data_block *data, int parameter_index){
   data_block::parameter_data_type parameter_value;

   if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR) {
                switch (parameter_index) {
                     case (escalator_data_block::INDEX_PARAM_BREAKING_PATH_VALUE) : {
                          parameter_value= data->get_signal_value( escalator_data_block::INDEX_SIGNAL_BREAKING_PATH_STATUS);
                         if (parameter_value==escalator_data_block::SIGNAL_VALUE_BLUE) return true;
                             return false;
                        }; //case (escalator_data_block::INDEX_PARAM_BREAKING_PATH_VALUE)
                     case (escalator_data_block::INDEX_PARAM_READY_VALUE) :{
                      parameter_value= data->get_signal_value( escalator_data_block::INDEX_SIGNAL_BLOCK_CIRCUT_STATUS);
                      if (parameter_value==escalator_data_block::SIGNAL_VALUE_BLUE) return true;
                           return false;
                    }; //case (escalator_data_block::INDEX_PARAM_STATE_VALUE)
                   };
        }; //if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR

   if(data->get_data_block_type_description()==data_block::DATA_BLOCK_SHAVR) {
         }; //if(data->get_data_block_type_description()==data_block::DATA_BLOCK_SHAVR

   if(data->get_data_block_type_description()==data_block::DATA_BLOCK_UDKU) {
                switch (parameter_index) {
                     case (udku_data_block::INDEX_PARAM_BREAKING_PATH_VALUE) : {
                         parameter_value= data->get_signal_value(udku_data_block::INDEX_SIGNAL_BREAKING_PATH_STATUS);
                         if (parameter_value==udku_data_block::SIGNAL_VALUE_GREEN) return true;
                             return false;
                        }; //case (udku_data_block::INDEX_PARAM_BREAKING_PATH_VALUE)

                     case (udku_data_block::INDEX_PARAM_MODE_VALUE) :{
                      parameter_value= data->get_signal_value(udku_data_block::INDEX_PARAM_MODE_VALUE);
                      switch (parameter_value) {
                         case system_settings::UDKU_MODE_GPSTOP:
                         case system_settings::UDKU_MODE_GPUP:
                         case system_settings::UDKU_MODE_GPDOWN:
                             return true;
                             break;
                         default:
                             return false;
                        }; //switch (parameter_value)
                    }; //case (udku_data_block::INDEX_PARAM_MODE_VALUE)

                     case (udku_data_block::INDEX_PARAM_ESCALATOR_SPEED_VALUE) :{
                      parameter_value= data->get_signal_value(udku_data_block::INDEX_PARAM_MODE_VALUE);
                      switch (parameter_value) {
                         case system_settings::UDKU_MODE_UP20:
                         case system_settings::UDKU_MODE_UP25:
                         case system_settings::UDKU_MODE_NOT7SEC:
                         case system_settings::UDKU_MODE_BREAK:
                         case system_settings::UDKU_MODE_SAMOHOD:
                             return false;
                             break;
                         default:
                             return true;
                        }; //switch (parameter_value)
                    }; //case (udku_data_block::INDEX_PARAM_ESCALATOR_SPEED_VALUE)


                   };
        }; //if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR


      return true;
};

string device_parameter::parameter_text_of_value(data_block *data,
                                                                               system_settings_spider *sys_sett,
                                                                               device_parameter *dev_par){

       data_block::parameter_data_type parameter_value;      
       ostringstream parameter_text;
       string parameter_name;

       try {
             parameter_value=data->get_parameter_value(dev_par->get_index());
       } catch (spider_exception spr_exc) {
            ostringstream exception_description;
            exception_description<<
              "In device_parameter::parameter_text_of_value(...) "<<
              spr_exc.get_description()<<
              " parameter index "<<dev_par->get_index();
           sys_sett->sys_message(system_settings::ERROR_MSG,
                                 exception_description.str());
           parameter_text <<"Error";
           return parameter_text.str();
      };

        if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR) {
                switch (dev_par->get_index()) {
                     case (escalator_data_block::INDEX_PARAM_MODE_VALUE) : {
                         parameter_text<<dev_par->get_name();
                         parameter_text<<sys_sett->get_escalator_mode_text( parameter_value );
                         break;
                         }; //case (INDEX_PARAM_STATE_VALUE)
                     case (escalator_data_block::INDEX_PARAM_READY_VALUE) :{
                         parameter_text.str();
                         parameter_text<<sys_sett->get_escalator_ready_text( parameter_value );
                         break;
                         }; //case (escalator_data_block::INDEX_PARAM_STATE_VALUE)
                       default: {
                        system_settings::strings_container parameters_names=sys_sett->get_parameters_in_messages_strings();
                        parameter_name=dev_par->get_name();
                         int find_pos=static_cast<int>(parameter_name.find(parameters_names[system_settings::PARAMETER_VALUE]));
                         vector<char> tmp_chars(20);
                         if (find_pos!=-1) {
                                  itoa (parameter_value, &tmp_chars[0], 10);
                                  parameter_name.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_VALUE].size(),
														   &tmp_chars[0]);
                              parameter_text<<parameter_name;
                          } else {
                                  parameter_text<<parameter_name<<parameter_value;
                          };
                      };
                };// switch (device_data_unit::get_index())
             } else  if(data->get_data_block_type_description()==data_block::DATA_BLOCK_SHAVR) {
                        system_settings::strings_container parameters_names=sys_sett->get_parameters_in_messages_strings();
                        parameter_name=dev_par->get_name();
                         int find_pos=static_cast<int>(parameter_name.find(parameters_names[system_settings::PARAMETER_VALUE]));
                         vector<char> tmp_chars(20);
                         if (find_pos!=-1) {
                                  itoa (parameter_value, &tmp_chars[0], 10);
                                  parameter_name.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_VALUE].size(),
														   &tmp_chars[0]);
                              parameter_text<<parameter_name;
                          } else {
                                  parameter_text<<parameter_name<<parameter_value;
                          };
             } else  if(data->get_data_block_type_description()==data_block::DATA_BLOCK_UDKU) {
                switch (dev_par->get_index()) {
                     case (udku_data_block::INDEX_PARAM_MODE_VALUE) : {
                         parameter_text<<dev_par->get_name();
                         parameter_text<<" "<<sys_sett->get_udku_mode_text( parameter_value );
                         break;
                         }; //case (udku_data_block::INDEX_PARAM_STATE_VALUE)
                     case (udku_data_block::INDEX_PARAM_DKSE_POSITION) :{
                         parameter_text<<dev_par->get_name();
                         parameter_text<<" "<<sys_sett->get_udku_position_text( parameter_value );
                         break;
                         }; //case (udku_data_block::INDEX_PARAM_DKSE_POSITION)
                     case (udku_data_block::INDEX_PARAM_ESCALATOR_TYPE) :{
                         parameter_text<<dev_par->get_name();
                         parameter_text<<" "<<sys_sett->get_udku_type_text( parameter_value );
                         break;
                         }; //case (udku_data_block::INDEX_PARAM_ESCALATOR_TYPE)

                       default: {
                        system_settings::strings_container parameters_names=sys_sett->get_parameters_in_messages_strings();
                        parameter_name=dev_par->get_name();
                         int find_pos=static_cast<int>(parameter_name.find(parameters_names[system_settings::PARAMETER_VALUE]));
                         vector<char> tmp_chars(20);
                         if (find_pos!=-1) {
                                  itoa (parameter_value, &tmp_chars[0], 10);
                                  parameter_name.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_VALUE].size(),
														   &tmp_chars[0]);
                              parameter_text<<parameter_name;
                          } else {
                              parameter_text<<parameter_name<<parameter_value;
                          };
                      };
                };// switch (device_data_unit::get_index())
             } else {   //if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR
                        system_settings::strings_container parameters_names=sys_sett->get_parameters_in_messages_strings();
                        parameter_name=dev_par->get_name();
                         int find_pos=static_cast<int>(parameter_name.find(parameters_names[system_settings::PARAMETER_VALUE]));
                         vector<char> tmp_chars(20);
                         if (find_pos!=-1) {
                                  itoa (parameter_value, &tmp_chars[0], 10);
                                  parameter_name.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_VALUE].size(),
														   &tmp_chars[0]);
                              parameter_text<<(parameter_name.c_str());
                          } else {
                                  parameter_text<<parameter_name<<parameter_value;
                          };
             };  //if(data->get_data_block_type_description()==data_block::DATA_BLOCK_ESCALATOR

       return parameter_text.str();
};

void device_signal::create_widget(
              ApDBase_t *widgets_dbase,
              PtWidget_t* parent,
              data_block *data,
              int column,
              int row) throw (spider_exception){
	vector<PtArg_t> args;
	PhPoint_t widget_pos;
	PtWidget_t *res_widget;
	string name;

     system_settings_spider *sys_sett=system_settings_spider::get_instance();

	name=device_data_unit::get_name();

    args.clear();
    args.resize(10);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, name.c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
	PtSetArg(&args[2], Pt_ARG_TEXT_IMAGE_SPACING,	 5, 0);
	PtSetArg(&args[3], Pt_ARG_WIDTH,	 system_settings_spider::DEVICE_PANEL_INDICATOR_WIDTH, 0);
	PtSetArg(&args[4], Pt_ARG_HEIGHT,	 system_settings_spider::DEVICE_PANEL_INDICATOR_HEIGHT, 0);
	PtSetArg(&args[5], Pt_ARG_LABEL_FLAGS, Pt_TRUE, Pt_SHOW_BALLOON);
	PtSetArg(&args[6], Pt_ARG_BALLOON_POSITION, Pt_BALLOON_RIGHT, 0);
	PtSetArg(&args[7], Pt_ARG_POINTER, this, 0);
	PtSetArg(&args[8], Pt_ARG_LABEL_BALLOON, inflate_data_block_balloon, 0);
	PtSetArg(&args[9], Pt_ARG_TEXT_FONT, sys_sett->get_large_font(), 0);

	widget_pos.x=system_settings_spider::DEVICE_PANEL_INDICATORS_MARGIN*(column+1)+
		  system_settings_spider::DEVICE_PANEL_INDICATOR_WIDTH*column;
	widget_pos.y=system_settings_spider::DEVICE_PANEL_INDICATORS_MARGIN*(row+1)+
	      system_settings_spider::DEVICE_PANEL_INDICATOR_HEIGHT*row;

	 res_widget = ApCreateDBWidget(widgets_dbase, 
	                                                    "block_grey_led",
	                                                    parent,
	                                                    &widget_pos,
	                                                    args.size(),
	                                                    &args[0]);

	if (res_widget==NULL)
          throw spider_exception("device_signal::create_widget can`t create widget");
	device_data_unit::set_widget(res_widget);
}

void device_signal::update_widget
                (data_block *data_blk,
                 system_settings_spider *sys_sett,
                 bool online) throw (spider_exception) {
	vector<PtArg_t> args(1);

    if (! device_data_unit::get_visibility()) return;

    if (online) {
        PtSetArg(&args[0], 
                       Pt_ARG_LABEL_IMAGE,
                       device_signal::signal_image(data_blk, sys_sett, get_index()),
                       0);
    } else {
        PtSetArg(&args[0], 
                       Pt_ARG_LABEL_IMAGE,
                       sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED),
                       0);
    };

	PtSetResources(device_data_unit::get_widget(), args.size(), &args[0]);
};


void device_parameter::create_widget(
              ApDBase_t *widgets_dbase,
              PtWidget_t* parent,
              data_block *data,
              int column,
              int row) throw (spider_exception){
     vector<PtArg_t> args;
	PhPoint_t widget_pos;
	PtWidget_t *parameter_widget;
	string widget_text;

     system_settings_spider *sys_sett=system_settings_spider::get_instance();

    args.clear();
    args.resize(29);
	PtSetArg(&args[0], Pt_ARG_WIDTH,	 system_settings_spider::DEVICE_PANEL_INDICATOR_WIDTH, 0);
	PtSetArg(&args[1], Pt_ARG_HEIGHT,	 system_settings_spider::DEVICE_PANEL_INDICATOR_HEIGHT, 0);
    PtSetArg(&args[2], Pt_ARG_COLOR,	 system_settings_spider::INDICATOR_DISABLED_TEXT, 0);
    PtSetArg(&args[3], Pt_ARG_FILL_COLOR,	 system_settings_spider::INDICATOR_DISABLED_FILL, 0);
    PtSetArg(&args[4], Pt_ARG_BORDER_CONTRAST,	 99, 0);
    PtSetArg(&args[5], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_FLAT_FILL);
    PtSetArg(&args[6], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_FULL_BORDERS);

    PtSetArg(&args[7], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_TOP_ETCH);
    PtSetArg(&args[8], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_TOP_OUTLINE);
    PtSetArg(&args[9], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_TOP_BEVEL);

    PtSetArg(&args[10], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_RIGHT_ETCH);
    PtSetArg(&args[11], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_RIGHT_OUTLINE);
    PtSetArg(&args[12], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_RIGHT_BEVEL);

    PtSetArg(&args[13], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_LEFT_ETCH);
    PtSetArg(&args[14], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_LEFT_OUTLINE);
    PtSetArg(&args[15], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_LEFT_BEVEL);

    PtSetArg(&args[16], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_BOTTOM_ETCH);
    PtSetArg(&args[17], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_BOTTOM_OUTLINE);
    PtSetArg(&args[18], Pt_ARG_BASIC_FLAGS,  Pt_TRUE, Pt_BOTTOM_BEVEL);

    PtSetArg(&args[19], Pt_ARG_BEVEL_WIDTH,  3, 0);

    PtSetArg(&args[20], Pt_ARG_MARGIN_HEIGHT,  0, 0);

    PtSetArg(&args[21], Pt_ARG_FLAGS,  Pt_TRUE, Pt_HIGHLIGHTED);

 	widget_text=device_data_unit::get_name();
	PtSetArg(&args[22], Pt_ARG_TEXT_STRING, widget_text.c_str(),0);

	widget_pos.x=system_settings_spider::DEVICE_PANEL_INDICATORS_MARGIN*(column+1)+
		  system_settings_spider::DEVICE_PANEL_INDICATOR_WIDTH*column;
	widget_pos.y=system_settings_spider::DEVICE_PANEL_INDICATORS_MARGIN*(row+1)+
	      system_settings_spider::DEVICE_PANEL_INDICATOR_HEIGHT*row;
	PtSetArg(&args[23], Pt_ARG_POS, &widget_pos, 0);
	PtSetArg(&args[24], Pt_ARG_LABEL_FLAGS, Pt_TRUE, Pt_SHOW_BALLOON);
	PtSetArg(&args[25], Pt_ARG_BALLOON_POSITION, Pt_BALLOON_INPLACE, 0);
	PtSetArg(&args[26], Pt_ARG_POINTER, this, 0);
	PtSetArg(&args[27], Pt_ARG_LABEL_BALLOON, inflate_data_block_balloon, 0);
	PtSetArg(&args[28], Pt_ARG_TEXT_FONT, sys_sett->get_large_font(), 0);

	parameter_widget=PtCreateWidget(PtLabel,
                                                                parent,
                                                                args.size(),
                                                                &args[0]);
	if (parameter_widget==NULL)
          throw spider_exception("parameter_widget::create_widget can`t create widget");

	device_data_unit::set_widget(parameter_widget);
}

void device_parameter::update_widget(data_block *data_blk,
                 system_settings_spider *sys_sett,
                 bool online) throw (spider_exception) {
	vector<PtArg_t> args;
	string parameter_text;

    if (! device_data_unit::get_visibility()) return;

    if (online) {
        args.resize(3);
        if (device_parameter::value_is_norma(data_blk, get_index())) {
                   PtSetArg(&args[0], 
                                 Pt_ARG_COLOR,
                                 system_settings_spider::INDICATOR_ENABLED_NORMA_TEXT,
                                 0);
                   PtSetArg(&args[1],
                                  Pt_ARG_FILL_COLOR,
                                  system_settings_spider::INDICATOR_ENABLED_NORMA_FILL,
                                  0);
         } else {
                   PtSetArg(&args[0], 
                                 Pt_ARG_COLOR,
                                 system_settings_spider::INDICATOR_ENABLED_NOT_NORMA_TEXT,
                                 0);
                   PtSetArg(&args[1],
                                  Pt_ARG_FILL_COLOR,
                                  system_settings_spider::INDICATOR_ENABLED_NOT_NORMA_FILL,
                                  0);
         };

          parameter_text=device_parameter::parameter_text_of_value(data_blk, sys_sett, this);
          PtSetArg(&args[2],
                        Pt_ARG_TEXT_STRING,
                        parameter_text.c_str(),
                         0);

    } else {
        args.resize(2);
        PtSetArg(&args[0], 
                       Pt_ARG_COLOR,
                       system_settings_spider::INDICATOR_DISABLED_FILL,
                       0);
        PtSetArg(&args[1],
                       Pt_ARG_FILL_COLOR,
                       system_settings_spider::INDICATOR_DISABLED_FILL,
                       0);
    };

	PtSetResources(device_data_unit::get_widget(), args.size(), &args[0]);
};

void metro_device_type::widget_create::operator() (int  data_unit_id){
	data_unit_iterator tmp_data_unit=	data_units_of_type->find(data_unit_id);

	if (tmp_data_unit==data_units_of_type->end()) return;

    if (! tmp_data_unit->second->get_visibility()) return;

	tmp_data_unit->second->create_widget(widgets_dbase, parent, data_blk, column, row);
    row ++;

    if (row>=system_settings_spider::DEVICE_PANEL_INDICATOR_ROWS_COUNT) {
         column++;
         row=0;
      };
}

void metro_device_type::panel_create::operator() (pair_blocks  block_pair) {
	vector<PtArg_t> args(3);
	PtWidget_t* panel;
	string block_title;
	block_pair.second.set_panel(NULL);

	block_title=block_pair.second.get_name();
	PtSetArg(&args[0], Pt_ARG_TITLE, block_title.c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_FILL_COLOR, 0x00BBC1D9, 0);
	PtSetArg(&args[2], Pt_ARG_OUTLINE_COLOR, 0x00A8A9D8, 0);

	panel = PtCreateWidget(PtPane, parent, args.size(), &args[0]);

	if (panel!=NULL) {
			widget_create  widget_creator(panel, widgets_dbase, data_units_of_type, data_blk);
			for_each(block_pair.second.id_begin(), block_pair.second.id_end(), widget_creator);
			block_pair.second.set_panel(panel);
		    PtRealizeWidget(panel);
		};
};


bool metro_device_type::create_panels(PtWidget_t* parent,
														ApDBase_t *widgets_dbase,
														data_block *data_blk){
	if (parent!=NULL) {
		panel_create  panel_creater(parent, widgets_dbase, data_blk, &type_data_units);
		for_each(this->type_blocks.begin(), this->type_blocks.end(), panel_creater);
		return true;
		} else return false;
};

metro_device_type::~metro_device_type () {
	data_unit_iterator tmp_iter=type_data_units.begin();

	while(tmp_iter!=type_data_units.end()) {
		  delete(tmp_iter->second);
          tmp_iter++;
	};

};
