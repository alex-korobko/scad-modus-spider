#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <iostream>

#include <vector>
#include <sstream>
#include <stdexcept>

using namespace std;
#include "definitions.h"

int activate_shutdown_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ) ;
int activate_ok_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ) ;

lines_container lines;
lines_container::size_type previos_lines_cbox_index=1;
string file_name;
PtWidget_t *line_choser_combobox, *main_window;

void save_line_parameters() throw (runtime_error) {
}

void save_settings_parameters() throw (runtime_error){
enum {LAST_SELECTED_LINE=0, ENTRIES_COUNT};

string entry_name;
ostringstream exception_description;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[LAST_SELECTED_LINE]="last_selected_line";

	if (PxConfigWriteInt( NULL,
			                    entries_names[LAST_SELECTED_LINE].c_str(),
   				                PXCONFIG_FMT_INT_DECIMAL,
			                    previos_lines_cbox_index)!=Pt_TRUE)
              throw runtime_error("save_escalator_parameters: PxConfigWriteInt LAST_SELECTED_LINE !=Pt_TRUE ");

}

void save() throw (runtime_error){
 	enum {LINE=0, SETTINGS, ENTRIES_COUNT};

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);
	sections_names[LINE]="line";
    sections_names[SETTINGS]="settings";

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ|PXCONFIG_WRITE)==Pt_FALSE ){
	       exception_description<<"Can`t open config file "<<file_name;
           throw runtime_error(exception_description.str());
	};

	section_name_c_str=PxConfigNextSection();
	try{
          while (section_name_c_str!=NULL) {
                 section_name=section_name_c_str;

                  if (section_name.compare(sections_names[LINE])==0) {
                         save_line_parameters();
			          };

                  if (section_name.compare(sections_names[SETTINGS])==0) {
                         save_settings_parameters();
			          };

			      section_name_c_str=PxConfigNextSection();
	       };	
    } catch (runtime_error run_err) {
       exception_description<<run_err.what();
       if  (PxConfigClose()==Pt_FALSE) {
 	       exception_description<<"\nCan`t close config file "<<file_name;
		 };
		throw runtime_error(exception_description.str());
  };

	if  (PxConfigClose()==Pt_FALSE) {
	       exception_description<<"Can`t close config file "<<file_name;
           throw runtime_error(exception_description.str());
	};
}

void load_line_parameters() throw (runtime_error){
enum {LINE_NAME=0, LINE_PASSWORD, LINE_SCRIPT, ENTRIES_COUNT};
string  exception_description;
string entry_name;
string line_name, line_password, line_script;
const char *entry_name_c_str;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[LINE_NAME]="line_name";
entries_names[LINE_PASSWORD]="line_password";
entries_names[LINE_SCRIPT]="line_script";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[LINE_NAME])==0) {
  	   line_name=&temp_str[0];
	} else if (entry_name.compare(entries_names[LINE_PASSWORD])==0) {
	    line_password=&temp_str[0];
	} else if (entry_name.compare(entries_names[LINE_SCRIPT])==0) {
	    line_script=&temp_str[0];
	} else {
           exception_description="Unrecognized config entry  ";
           exception_description+=entry_name;
           throw runtime_error(exception_description);
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while(entry_name_c_str!=NULL)

	if (! line_name.empty() &&
        ! line_password.empty() &&
        ! line_script.empty()) {
           lines.push_back(line_description(line_name, line_password, line_script));
	} else {
       exception_description="Not found all of required entries for msg_dict_container";
       if (line_name.empty())
             exception_description+=" line name is empty";
       if (line_password.empty())
             exception_description+=" line password is empty";
       if (line_script.empty())
             exception_description+=" line script is empty";

       throw runtime_error(exception_description);
	};

};

void load_settings_parameters() throw (runtime_error){
enum {LAST_SELECTED_LINE=0, ENTRIES_COUNT};
string  exception_description;
string entry_name;
int last_selected_line(-1);
const char *entry_name_c_str;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[LAST_SELECTED_LINE]="last_selected_line";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[LAST_SELECTED_LINE])==0) {
  	   last_selected_line=atoi(&temp_str[0]);
       if (last_selected_line>0)
           previos_lines_cbox_index=static_cast<lines_container::size_type>(last_selected_line);
	} else {
           exception_description="Unrecognized config entry  ";
           exception_description+=entry_name;
           throw runtime_error(exception_description);
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while(entry_name_c_str!=NULL)

};


void load_parameters() throw (runtime_error) {
 	enum {LINE=0, SETTINGS, ENTRIES_COUNT};
	ostringstream exception_description;

    lines.clear();

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";
    sections_names[SETTINGS]="settings";

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
        cout<<"Can`t open config file "<<file_name<<endl;
        return;
	};

   try {
     section_name_c_str=PxConfigNextSection();
     while (section_name_c_str!=NULL) {
           section_name=section_name_c_str;

           if (section_name.compare(sections_names[LINE])==0) {
                              load_line_parameters ();
            } else if (section_name.compare(sections_names[SETTINGS])==0) {
                              load_settings_parameters ();
             } else {
                   string  err_descr="Unrecognized config section ";
                   err_descr+=section_name;
                   throw runtime_error(err_descr);
              };
            section_name_c_str=PxConfigNextSection();
       }; //while (section_name_c_str!=NULL)
    } catch (runtime_error run_err) {
           string message=run_err.what();
            if  (PxConfigClose()!=Pt_TRUE) {
                  message+="  Can`t close file ";
                  message+=file_name;
                 };
             throw runtime_error(message);
    };

     if  (PxConfigClose()!=Pt_TRUE) {
          string message("Can`t close file ");
          message+=file_name;
          throw runtime_error(message);
       };
};

void usage(string program_name) {
cout<<"Usage :\n"
         <<program_name<<" configuration_file\n"
         <<"\twhere :\n"
         <<"\t\tconfiguration_file file with configuration settings \n"
         <<"\t\t\tby example lines_switcher.conf\n"
        <<endl;
};


int main (int argc , char* argv[]) {
PtWidget_t *current_widget;
vector<PtArg_t> args;
vector<PtCallback_t> widget_callbacks;
vector<const char*> combobox_item(1);
PtCallback_t tmp_callback;
PhPoint_t widget_position;
PhRect_t  widget_extent;
int button_width=80, window_height=100, window_width=300;


try {

if (argc<2) {
      if (argc<1) {
              usage("lines_switcher");
          } else {
              usage(argv[0]);
           };
      throw runtime_error("Too less parameters to run");
   };

file_name=argv[1];
load_parameters();

args.clear();
args.resize(17);
widget_position.x=400;
widget_position.y=300;
PtSetArg(&args[0], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[1], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_CLOSE);
PtSetArg(&args[2], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_TOFRONT );
PtSetArg(&args[3], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_TOBACK);
PtSetArg(&args[4], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_MAX);
PtSetArg(&args[5], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_BACKDROP);
PtSetArg(&args[6], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_RESIZE);
PtSetArg(&args[7],  Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_CONSWITCH);
PtSetArg(&args[8], Pt_ARG_WIDTH,
         window_width, 0);
PtSetArg(&args[9], Pt_ARG_HEIGHT,
         window_height, 0);
//PtSetArg(&args[9], Pt_ARG_WINDOW_RENDER_FLAGS,
//         Pt_FALSE, Ph_WM_RENDER_HELP);
PtSetArg(&args[10], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_COLLAPSE);
PtSetArg(&args[11], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_MENU);
PtSetArg(&args[12], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_MIN);
PtSetArg(&args[13], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_RESIZE);
PtSetArg(&args[14], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_TITLE);
PtSetArg(&args[15], Pt_ARG_CURSOR_TYPE,
         Ph_CURSOR_BIG_POINTER, 0);
PtSetArg(&args[16], Pt_ARG_CURSOR_COLOR,
         0xf33ff5, 0);

main_window=PtAppInit( NULL,
                0,
                NULL,
                args.size(),
                &args[0]);

if (main_window==NULL)  throw runtime_error("main window is NULL");

args.clear();
args.resize(2);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
         "Линия : ", 0);
widget_position.x=10;
widget_position.y=5;
PtSetArg(&args[1], Pt_ARG_POS,
         &widget_position, 0);
current_widget=PtCreateWidget( PtLabel,
                main_window,
                args.size(),
                &args[0]);
if (current_widget==NULL)  throw runtime_error("Can`t create line label");
PtExtentWidget(current_widget);
PtWidgetExtent(current_widget, &widget_extent);

widget_position.y = widget_extent.ul.y;
widget_position.x = widget_extent.lr.x;
widget_position.x += 5;

args.clear();
args.resize(4);
PtSetArg(&args[0], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[1], Pt_ARG_WIDTH,
         200, 0);
PtSetArg(&args[2], Pt_ARG_TEXT_FLAGS,
         Pt_FALSE, Pt_EDITABLE);
PtSetArg(&args[3], Pt_ARG_TEXT_FLAGS,
         Pt_FALSE, Pt_INSERT_MODE);

line_choser_combobox=PtCreateWidget( PtComboBox,
                main_window,
                args.size(),
                &args[0]);
if (line_choser_combobox==NULL)  throw runtime_error("Can`t create line combobox");
for(lines_container::size_type i=0; i<lines.size(); i++) {
 combobox_item[0]=lines[i].line_name.c_str();
 PtListAddItems(line_choser_combobox,
                    &combobox_item[0],
                    1,
                    0);
 };
if (lines.size()>=previos_lines_cbox_index)
	PtSetResource(line_choser_combobox, Pt_ARG_CBOX_SEL_ITEM, previos_lines_cbox_index,0);

widget_position.x=window_width-button_width-20;
widget_position.y+=50;

tmp_callback.event_f=activate_ok_button;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);

args.clear();
args.resize(4);
PtSetArg(&args[0], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[1], Pt_ARG_WIDTH,
         button_width, 0);
PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
        "OK", 0);
PtSetArg(&args[3], Pt_CB_ACTIVATE,
        &widget_callbacks[0], widget_callbacks.size());

current_widget=PtCreateWidget( PtButton,
                main_window,
                args.size(),
                &args[0]);
if (current_widget==NULL)  throw runtime_error("Can`t create line combobox");

PtExtentWidget(current_widget);
PtWidgetExtent(current_widget, &widget_extent);

widget_position.y=widget_extent.ul.y;
widget_position.x=widget_extent.ul.x-button_width-30;

tmp_callback.event_f=activate_shutdown_button;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);

args.clear();
args.resize(4);
PtSetArg(&args[0], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[1], Pt_ARG_WIDTH,
         button_width, 0);
PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
        "Выключить", 0);
PtSetArg(&args[3], Pt_CB_ACTIVATE,
        &widget_callbacks[0], widget_callbacks.size());

current_widget=PtCreateWidget( PtButton,
                main_window,
                args.size(),
                &args[0]);
if (current_widget==NULL)  throw runtime_error("Can`t create line combobox");


PtRealizeWidget(main_window);
PtMainLoop();

} catch (runtime_error exc) {
   cout<<"Raised exception "<<exc.what()<<endl;
};

return (EXIT_SUCCESS); //make compiler happ
};






