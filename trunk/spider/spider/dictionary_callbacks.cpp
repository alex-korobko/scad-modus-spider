#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>

#include <hash_map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

using namespace std;

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "dispatchers_persons_container.h"

//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            ApInfo_t *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//local functions
static int activate_exit_button_in_dispatchers_window ( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){

   dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
   if (dispatchers==NULL) {
        cout<<"In activate_exit_button_in_dispatchers_window: Can`t get instance of dispatcher_persons_container"<<endl;
        return( Pt_HALT);
        };

   dispatchers->set_dispatchers_list(NULL);
   dispatchers->set_dispatchers_first_name_text(NULL);
   dispatchers->set_dispatchers_middle_name_text(NULL);
   dispatchers->set_dispatchers_last_name_text(NULL);

    return Pt_CONTINUE;
};

static int
selection_dispatchers_list(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
PtListCallback_t *list_callback_data=static_cast<PtListCallback_t*>(cbinfo->cbdata);
ushort_t *sel_array=list_callback_data->sel_array;
int sel_item_count=list_callback_data->sel_item_count;

if (sel_item_count<=0) return 0;

vector<ushort_t> selected_idexes(sel_item_count);
memcpy(&selected_idexes[0], sel_array, sel_item_count*sizeof(ushort_t));
cout<<"sel_item_count "<<sel_item_count<<endl;


dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL)  return Pt_HALT;

dispatcher_persons_container::iterator disp_iter=dispatchers->begin();
if (dispatchers->size()<selected_idexes[0]) return Pt_HALT;

advance(disp_iter, selected_idexes[0]-1);
string text;

text=disp_iter->get_first_name();
text.append(text.size(),' ');
if (dispatchers->get_dispatchers_first_name_text()!=NULL) 
    PtSetResource(dispatchers->get_dispatchers_first_name_text(),
                             Pt_ARG_TEXT_STRING, text.c_str(),0);

text=disp_iter->get_middle_name();
text.append(text.size(),' ');
if (dispatchers->get_dispatchers_middle_name_text()!=NULL) 
    PtSetResource(dispatchers->get_dispatchers_middle_name_text(),
                             Pt_ARG_TEXT_STRING, text.c_str(),0);

text=disp_iter->get_last_name();
text.append(text.size(),' ');
if (dispatchers->get_dispatchers_last_name_text()!=NULL) 
    PtSetResource(dispatchers->get_dispatchers_last_name_text(),
                             Pt_ARG_TEXT_STRING, text.c_str(),0);

return Pt_CONTINUE;
};


static int activate_new_dispatcher_window ( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
    return Pt_CONTINUE;
};

static int activate_edit_dispatcher_window ( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
    return Pt_CONTINUE;
};

static int activate_delete_dispatcher_window ( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
    return Pt_CONTINUE;
};


static void  create_dispatchers_panel(PtWidget_t *dictionary_panel_group,
                                                 int dialog_wnd_width,
                                                 int dialog_wnd_height,
                                                 int buttons_panel_height) throw (spider_exception){
const unsigned int button_width=120;

vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
PtWidget_t *dispatchers_panel, *current_widget;

dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL) {
      cout<<"In activate_dictionary_window: Can`t get instance of dispatcher_persons_container"<<endl;
      return ;
      };

        //dispatchers panel
		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_TITLE,
                             "Диспетчеры", 0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                             Pt_FALSE, Pt_HIGHLIGHTED);

         dispatchers_panel=PtCreateWidget(PtPane,
                                                     dictionary_panel_group,
                                                     args.size(),
                                                     &args[0]);
         if (dispatchers_panel==NULL)
                  throw spider_exception("dispatchers_panel is NULL");

       //list with dispatchers
		args.clear();
		args.resize(5);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          (dialog_wnd_width-18),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-150, 0);
         PtSetArg(&args[3], Pt_ARG_SELECTION_MODE,
                             Pt_SELECTION_MODE_SINGLE, 0);
          callbacks.clear();
          tmp_callback.event_f=selection_dispatchers_list;
          tmp_callback.data=NULL;
          callbacks.push_back(tmp_callback);
          PtSetArg(&args[4],  Pt_CB_SELECTION,
			           &callbacks[0],
                       callbacks.size());

         current_widget=PtCreateWidget(PtList,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("dispatchers_list is NULL");
          dispatchers->set_dispatchers_list(current_widget);


         vector<const char*> item_cont(1);
         vector<unsigned short> sel_indexes;
         dispatcher_persons_container::iterator disp_iter=dispatchers->begin();
         while(disp_iter!=dispatchers->end()) {
              item_cont[0]= (disp_iter->get_last_name()+" "+disp_iter->get_first_name()+" "+disp_iter->get_middle_name()).c_str();
                  if( PtListAddItems( dispatchers->get_dispatchers_list(),
                    &item_cont[0],
                    item_cont.size(), 0)!=0)
                            throw spider_exception("error in PtListAddItems(...) for dispatchers");
              disp_iter++;
           };

        string first_name, middle_name, last_name;
        if (dispatchers->size()>0) {
         PtListSelectPos(dispatchers->get_dispatchers_list(), 1);
         first_name=dispatchers->begin()->get_first_name();
         first_name.append(first_name.size(), ' ');
         middle_name=dispatchers->begin()->get_middle_name();
         middle_name.append(middle_name.size(), ' ');
         last_name=dispatchers->begin()->get_last_name();
         last_name.append(last_name.size(), ' ');
         };

		 args.clear();
		 args.resize(2);
         widget_position.x=0;
         widget_position.y=dialog_wnd_height-buttons_panel_height-145;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Имя :", 0);
         current_widget=PtCreateWidget(PtLabel,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label first name is NULL");

		 args.clear();
		 args.resize(4);
         widget_position.x=80;
         widget_position.y=dialog_wnd_height-buttons_panel_height-147;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                          first_name.c_str(), 0);
         PtSetArg(&args[2], Pt_ARG_WIDTH,
                            200, 0);
         PtSetArg(&args[3], Pt_ARG_HEIGHT,
                            30, 0);
         current_widget=PtCreateWidget(PtText,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("edit first name is NULL");
          dispatchers->set_dispatchers_first_name_text(current_widget);

		 args.clear();
		 args.resize(2);
         widget_position.x=0;
         widget_position.y=dialog_wnd_height-buttons_panel_height-115;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Отчество :", 0);
         current_widget=PtCreateWidget(PtLabel,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label middle name is NULL");

		 args.clear();
		 args.resize(4);
         widget_position.x=80;
         widget_position.y=dialog_wnd_height-buttons_panel_height-117;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                          middle_name.c_str(), 0);
         PtSetArg(&args[2], Pt_ARG_WIDTH,
                            200, 0);
         PtSetArg(&args[3], Pt_ARG_HEIGHT,
                            30, 0);
         current_widget=PtCreateWidget(PtText,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("edit middle name is NULL");
          dispatchers->set_dispatchers_middle_name_text(current_widget);

		 args.clear();
		 args.resize(2);
         widget_position.x=0;
         widget_position.y=dialog_wnd_height-buttons_panel_height-85;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Фамилия :", 0);
         current_widget=PtCreateWidget(PtLabel,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label last name is NULL");

		 args.clear();
		 args.resize(4);
         widget_position.x=80;
         widget_position.y=dialog_wnd_height-buttons_panel_height-87;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                          last_name.c_str(), 0);
         PtSetArg(&args[2], Pt_ARG_WIDTH,
                            200, 0);
         PtSetArg(&args[3], Pt_ARG_HEIGHT,
                            30, 0);
         current_widget=PtCreateWidget(PtText,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("edit last name is NULL");
          dispatchers->set_dispatchers_last_name_text(current_widget);

         //buttons for dispatcher editing
		 args.clear();
		 args.resize(4);
         widget_position.x=dialog_wnd_width-button_width-40;
         widget_position.y=dialog_wnd_height-buttons_panel_height-145;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Новый", 0);

		callbacks.clear();
		tmp_callback.event_f=activate_new_dispatcher_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[2],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                       callbacks.size());
         PtSetArg(&args[3], Pt_ARG_WIDTH,
                            button_width, 0);

         current_widget=PtCreateWidget(PtButton,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("button new dispatcher is NULL");

		 args.clear();
		 args.resize(4);
         widget_position.x=dialog_wnd_width-button_width-40;
         widget_position.y=dialog_wnd_height-buttons_panel_height-117;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Изменить", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_edit_dispatcher_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[2],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                       callbacks.size());
         PtSetArg(&args[3], Pt_ARG_WIDTH,
                            button_width, 0);

         current_widget=PtCreateWidget(PtButton,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("button edit dispatcher is NULL");

		 args.clear();
		 args.resize(4);
         widget_position.x=dialog_wnd_width-button_width-40;
         widget_position.y=dialog_wnd_height-buttons_panel_height-85;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Удалить", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_delete_dispatcher_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[2],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                       callbacks.size());
         PtSetArg(&args[3], Pt_ARG_WIDTH,
                            button_width, 0);


         current_widget=PtCreateWidget(PtButton,
                                                  dispatchers_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("button delete dispatcher is NULL");
};

int
activate_dictionary_window( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window, *common_panel, *dictionary_panel_group, 
                     *buttons_panel,  *current_button;

vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title;

unsigned int dialog_wnd_height=350, 
                     dialog_wnd_width=850,
                     common_panel_height=dialog_wnd_height-2,
                     buttons_panel_height=40,
                     button_in_panel_width=90,
                     button_in_panel_height=buttons_panel_height-10;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_dictionary_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL) {
     cout<<"In activate_dictionary_window: Can`t get instance of dispatcher_persons_container"<<endl;
     return( Pt_HALT);
    };

try {
		args.clear();
		args.resize(15);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, "Справочная информация",0);
		PtSetArg(&args[3], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_RESIZE);
		PtSetArg(&args[4], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_TOBACK);
		PtSetArg(&args[5], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MAX);
		PtSetArg(&args[6], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MENU);
		PtSetArg(&args[7], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_COLLAPSE );
		PtSetArg(&args[8], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MAX );
		PtSetArg(&args[9], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MENU  );
		PtSetArg(&args[10], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_CLOSE  );
		PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_COLLAPSE );

		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_dispatchers_window;
        tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
        tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

        PtSetArg(&args[12],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                       callbacks.size());

		PtSetArg(&args[13], Pt_ARG_CURSOR_TYPE,
  				       Ph_CURSOR_BIG_POINTER, 0);
		PtSetArg(&args[14], Pt_ARG_CURSOR_COLOR,
  				       0xf33ff5, 0);


		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");

        //common
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, common_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         common_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (common_panel==NULL)
                  throw spider_exception("common_panel is NULL");

        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-4,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-8,0);
          widget_position.x=0;
          widget_position.y=common_panel_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
                                                     common_panel,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");

        //dictionary panel group
		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_HEIGHT,
                           common_panel_height-buttons_panel_height-4, 0);
         PtSetArg(&args[2], Pt_ARG_WIDTH,
                          dialog_wnd_width-8, 0);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[5], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[6], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
          dictionary_panel_group=PtCreateWidget(PtPanelGroup,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
         if (dictionary_panel_group==NULL)
                  throw spider_exception("dictionary_panel_group is NULL");

         create_dispatchers_panel(dictionary_panel_group, 
                                                  dialog_wnd_width,
                                                  dialog_wnd_height,
                                                  buttons_panel_height);

         //buttons in button panel
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-20;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_dispatchers_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Close\" in buttons_panel is NULL");

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_dictionary_window: "+spr_exc.get_description());

   dispatchers->set_dispatchers_list(NULL);
   dispatchers->set_dispatchers_first_name_text(NULL);
   dispatchers->set_dispatchers_middle_name_text(NULL);
   dispatchers->set_dispatchers_last_name_text(NULL);

     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return Pt_CONTINUE;
};

