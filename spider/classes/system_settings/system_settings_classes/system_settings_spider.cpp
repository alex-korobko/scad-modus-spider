#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include <Ap.h>
#include <Pt.h>


using namespace std;
#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

system_settings_spider* system_settings_spider::system_settings_spider_instance=NULL;

system_settings_spider* system_settings_spider::get_instance(){
        if (system_settings_spider_instance==NULL) {
                    system_settings_spider_instance=new system_settings_spider();
             };
   return system_settings_spider_instance;
};

system_settings_spider::system_settings_spider() : 
 	widget_dbase(NULL),
    main_window(NULL),
    map_container(NULL),
    images(IMAGES_COUNT),
	start_wav_file_name("sounds/wp_start.wav"),
	report_import_directory("/home/kor/tmp/")
 {};


void system_settings_spider::load_settings() throw (spider_exception) 
{
enum {REPORT_DIR=0,  ENTRIES_COUNT};
ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[REPORT_DIR]="report_directory";

entry_name_c_str=PxConfigNextString(&temp_str[0],temp_str.size());

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[REPORT_DIR])==0) {
			report_import_directory = &temp_str[0];
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
            throw spider_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};

};

void system_settings_spider::prepare_visualization(
           ApDBase_t* new_widget_dbase) throw (spider_exception){
		ostringstream exception_description;
		widget_dbase=new_widget_dbase;
		PhImage_t *img_ptr;
        
       img_ptr=ApGetImageRes(widget_dbase, "arrow_green_up");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_UP");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREEN_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_green_up_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_UP_COMMAND");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREEN_UP_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_green_down");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_DOWN");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
       images[GREEN_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_green_down_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_DOWN_COMMAND");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREEN_DOWN_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_green_up_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_S_UP");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREEN_S_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_green_down_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREEN_S_DOWN");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREEN_S_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_up");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_UP");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_up_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_UP_COMMAND");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_UP_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_down");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_DOWN");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_down_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_DOWN_COMMAND");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_DOWN_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_up_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_S_UP");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_S_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_red_down_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image RED_S_DOWN");
//        img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[RED_S_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_up");
		if (img_ptr==NULL)
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_UP");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_up_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_UP_COMMAND");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_UP_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_down");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_DOWN");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_down_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_DOWN_COMMAND");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_DOWN_COMMAND]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_up_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_S_UP");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_S_UP]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "arrow_yellow_down_small");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image YELLOW_S_DOWN");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[YELLOW_S_DOWN]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "reverse");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image REVERSE");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[REVERSE]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "block");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image BLOCK");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[BLOCK]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "exception");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image EXCEPTION");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[EXCEPTION]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "tu");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image TU");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[TU]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "offline");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image OFFLINE");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[OFFLINE]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "grey_off");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image GREY_FLAT");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[GREY_FLAT]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "shavr_disabled_1_disabled_2");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image SHAVR_DISABLED_1_DISABLED_2");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[SHAVR_DISABLED_1_DISABLED_2]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "shavr_disabled_1_enabled_2");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image SHAVR_DISABLED_1_ENABLED_2");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[SHAVR_DISABLED_1_ENABLED_2]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "shavr_enabled_1_disabled_2");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image SHAVR_ENABLED_1_DISABLED_2");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[SHAVR_ENABLED_1_DISABLED_2]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "shavr_enabled_1_enabled_2");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image SHAVR_ENABLED_1_ENABLED_2");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[SHAVR_ENABLED_1_ENABLED_2]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "shavr_command");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image SHAVR_COMMAND");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
         images[SHAVR_COMMAND]=*img_ptr;
         
		img_ptr=ApGetImageRes(widget_dbase, "block_green_led");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image BLOCK_GREEN_LED");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[BLOCK_GREEN_LED]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "block_red_led");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image BLOCK_RED_LED");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[BLOCK_RED_LED]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "block_grey_led");
		if (img_ptr==NULL)
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image BLOCK_GREY_LED");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[BLOCK_GREY_LED]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "block_blue_led");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image BLOCK_BLUE_LED");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[BLOCK_BLUE_LED]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "errorMsg");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image MSG_ERROR");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[MSG_ERROR]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "infoMsg");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image MSG_INFO");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[MSG_INFO]=*img_ptr;

		img_ptr=ApGetImageRes(widget_dbase, "warnMsg");
		if (img_ptr==NULL) 
		         throw spider_exception("system_settings_spider::prepare_visualization can`t get image MSG_WARN");
//       img_ptr->flags=Ph_RELEASE_IMAGE | Ph_RELEASE_PALETTE |Ph_RELEASE_TRANSPARENCY_MASK | Ph_RELEASE_GHOST_BITMAP;
        images[MSG_WARN]=*img_ptr;

		font_for_messages_large.resize(MAX_FONT_TAG);
		string font_name("Helvetica");
		int  font_size(12);

		if (PfGenerateFontName ( font_name.c_str(),
												PF_STYLE_ANTIALIAS | PF_STYLE_BOLD,
												font_size,
												&font_for_messages_large[0]) ==NULL) {
				font_for_messages_large.erase(font_for_messages_large.begin(), font_for_messages_large.end());

                exception_description<<"ERROR: Can`t generate font name for font "<<font_name<<" size "<<font_size;
                throw spider_exception(exception_description.str());
                };

		font_for_messages_small.resize(MAX_FONT_TAG);		
		font_name="Arial";
		font_size=10;
		
		if (PfGenerateFontName ( font_name.c_str(),
												PF_STYLE_ANTIALIAS,
												font_size,
												&font_for_messages_small[0]) ==NULL)  {
                  font_for_messages_small.erase(font_for_messages_small.begin(), font_for_messages_small.end());						

                  exception_description<<"ERROR: Can`t generate font name for font "<<font_name<<" size "<<font_size;
                  throw spider_exception(exception_description.str());
				};

}

PhImage_t* system_settings_spider::get_image (images_size_type image_index){
		if (image_index>=0 && 
			image_index<images.size()) {
				return(&images[image_index]);
			} else {
				return(&images[GRAY_FLAT]); //what can I do???
			};
};

void system_settings_spider::message_window (
             const int type,
             const string mess_text,
             const bool is_detached){
PhImage_t *window_icon;
string window_title;

	switch(type){
		case system_settings::ERROR_MSG:
			window_icon=this->get_image(MSG_ERROR);
			window_title="Ошибка !";
			break;
		case system_settings::INFO_MSG:
			window_icon=this->get_image(MSG_INFO);
			window_title="Информация";
      		break;
		case system_settings::WARN_MSG:
			window_icon=this->get_image(MSG_WARN);
			window_title="Внимание !";
      		break;
      	default:
			window_icon=this->get_image(MSG_ERROR);
			window_title="Важное сообщение";
			break;
	};

	if (!font_for_messages_small.empty() ||
        !font_for_messages_large.empty()) {
	is_detached ?
	PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					(const char*) &font_for_messages_small[0], 
					"Ok", 
					(const char*) &font_for_messages_large[0],
					Pt_ESC_DISABLE | Pt_RIGHT|Pt_RELATIVE) 
					 : 
	PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					(const char*) &font_for_messages_small[0], 
					"Ok", 
					(const char*) &font_for_messages_large[0],
					Pt_ESC_DISABLE | Pt_MODAL | Pt_CENTER|Pt_RELATIVE) ;
	} else {
            this->sys_message(system_settings::ERROR_MSG,
                               "ERROR: Can`t found small or large font for message window");

	is_detached ?
		PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					NULL, 
					"Ok", 
					NULL,
					Pt_ESC_DISABLE | Pt_RIGHT|Pt_RELATIVE)
					 : 
		PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					NULL, 
					"Ok", 
					NULL,
					Pt_ESC_DISABLE | Pt_MODAL | Pt_CENTER|Pt_RELATIVE);

	}; 
};

system_settings::types_of_system_settings 
          system_settings_spider::get_system_settings_type(){return system_settings::SPIDER_SYSTEM_SETTINGS;};

