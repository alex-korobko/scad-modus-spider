#ifndef __SYSTEM_SETTINGS_SPIDER_H__
#define __SYSTEM_SETTINGS_SPIDER_H__

class system_settings_spider : public system_settings {
private:
typedef vector<PhImage_t> images_container;
typedef images_container::size_type images_size_type;

typedef vector<char> chars_container;

ApDBase_t *widget_dbase;
PtWidget_t *main_window;
PtWidget_t *map_container;

chars_container font_for_messages_small;
chars_container font_for_messages_large;
chars_container font_for_menu;

images_container images;
string start_wav_file_name;
string report_import_directory;

system_settings_spider();

static system_settings_spider* system_settings_spider_instance;
public:

virtual ~system_settings_spider() {};

//Enumerated constants

	// PtGenLists and childs parameters
	enum{
				COLUMN_LEFT_MARGIN =5,
				ROW_HEIGHT=40,
                MAX_CHARS_COUNT_IN_ROW=100
			};

	// images
   	enum {GREEN_UP=0,
                GREEN_UP_COMMAND,
   				GREEN_DOWN,
   				GREEN_DOWN_COMMAND,
   				RED_UP,
   				RED_UP_COMMAND,
   				RED_DOWN,
   				RED_DOWN_COMMAND,
   				YELLOW_UP,
   				YELLOW_UP_COMMAND,
   				YELLOW_DOWN,
   				YELLOW_DOWN_COMMAND,

   				GREEN_S_UP,
   				GREEN_S_DOWN,
   				RED_S_UP,
   				RED_S_DOWN,
   				YELLOW_S_UP,
   				YELLOW_S_DOWN,

                REVERSE,

   				BLOCK,
   				EXCEPTION,
   				OFFLINE,
   				TU,
                GREY_FLAT,

   				SHAVR_DISABLED_1_DISABLED_2,
   				SHAVR_DISABLED_1_ENABLED_2,
   				SHAVR_ENABLED_1_DISABLED_2,
   				SHAVR_ENABLED_1_ENABLED_2,
   				SHAVR_COMMAND,
   				
                  GRAY_FLAT,

                BLOCK_GREEN_LED,
   				BLOCK_RED_LED,
   				BLOCK_GREY_LED,
   				BLOCK_BLUE_LED,

   				MSG_ERROR,
   				MSG_INFO,
   				MSG_WARN,

   				IMAGES_COUNT
   				};

//Colors 
	enum {
				COLOR_RED=0x00EE0000,
				COLOR_BLUE=0x000000AA,
				COLOR_GREEN=0x0000EE00,
				COLOR_BLACK=0x00000000, 
				COLOR_GRAY=0x00909090, 
				COLOR_WHITE=0x00FFFFFF, 

				NO_BLOCK_COLOR=COLOR_GRAY,
				BLOCK_COLOR=COLOR_RED,

				INDICATOR_ENABLED_NORMA_FILL=0x247428,
				INDICATOR_ENABLED_NORMA_TEXT=0xddFFdd,
                INDICATOR_ENABLED_NOT_NORMA_FILL=0xb60000,
                INDICATOR_ENABLED_NOT_NORMA_TEXT=0xffb621,

				INDICATOR_DISABLED_FILL=COLOR_GRAY,
				INDICATOR_DISABLED_TEXT=COLOR_GRAY
};

//other
	enum { 
             SPACE_BETWEEN_METRO_DEVICES_IN_STATION=10,
             TOP_BOTTOM_MARGIN_OF_STATION=20,
             POWER_LINE_WIDTH=5,

             DEVICE_PANEL_INDICATOR_WIDTH=150,
             DEVICE_PANEL_INDICATOR_HEIGHT=28,
             DEVICE_PANEL_INDICATORS_MARGIN=5,
             DEVICE_PANEL_INDICATOR_ROWS_COUNT=9,

             TOP_PANEL_HEIGHT_IN_MAIN_WINDOW=45,

             PULSE_CODE_UPDATE_DEVICE= _PULSE_CODE_MINAVAIL+10,
             PULSE_CODE_TIMER_INTERVAL=_PULSE_CODE_MINAVAIL+11,
             PHOTON_THREAD_PULSE=SIGEV_PULSE_PRIO_INHERIT,

             MESSAGE_TYPE_DISPATCHER_DOIT=5
};

// initialization
	void prepare_visualization(ApDBase_t* new_widget_dbase) throw (spider_exception);

// getters
	ApDBase_t* get_widgets_dbase() {return(widget_dbase);};

	PtWidget_t* get_main_window() {return(main_window);};
	void set_main_window(PtWidget_t *new_main_window) {main_window=new_main_window;};

	PtWidget_t* get_map_container() {return(map_container);};
	void set_map_container(PtWidget_t *new_map_container) {map_container=new_map_container;};

	PhImage_t* get_image (images_size_type image_index);
	const string& get_start_wav_file_name() const {return start_wav_file_name;};
	const string& get_report_import_directory() const {return report_import_directory;};

// windows
	void message_window(
             const int type,
             const string mess_text,
             const bool is_detached=false);		

// working with fonts
	bool small_font_prepared() {return (!font_for_messages_small.empty());};
	const char* get_small_font() {return this->small_font_prepared()? &font_for_messages_small[0] : NULL ;};

	bool large_font_prepared() {return (!font_for_messages_large.empty());};
	const char* get_large_font(){return this->large_font_prepared()?&font_for_messages_large[0] : NULL;};

	bool menu_font_prepared() {return (!font_for_menu.empty());};
	const char* get_menu_font(){return this->menu_font_prepared()?&font_for_menu[0] : NULL;};

//virtual metods
virtual system_settings::types_of_system_settings get_system_settings_type();

//load settings from file
	void load_settings()  throw (spider_exception);
//static metods
     static system_settings_spider* get_instance();

};

#endif
