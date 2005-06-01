#ifndef __SYSTEM_SETTINGS_SPIDER_H__
#define __SYSTEM_SETTINGS_SPIDER_H__

class system_settings_spider : public system_settings {
private:
typedef vector<PhImage_t> images_container;
typedef images_container::size_type images_size_type;

ApDBase_t *widget_dbase;
PtWidget_t *main_window, *escalator_panel;

unsigned_chars_container font_for_messages_small;
unsigned_chars_container font_for_messages_large;

images_container images;
dword paneled_escalator_id;
string start_wav_file_name;

public:

//Enumerated constants

	// PtGenLists and childs parameters
	enum{
				COLUMN_LEFT_MARGIN =5,
				ROW_HEIGHT=18
			};

	// images
   	enum {GREEN_UP=0, 
   				GREEN_DOWN,
   				RED_UP,
   				RED_DOWN,
   				YELLOW_UP,
   				YELLOW_DOWN,
   				GREEN_S_UP,
   				GREEN_S_DOWN,
   				RED_S_UP,
   				RED_S_DOWN,
   				YELLOW_S_UP,
   				YELLOW_S_DOWN,
   				GREY_STOP,
   				GREY_OFF,
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
				COLOR_GREEN=0x00EE0000,
				COLOR_BLACK=0x00000000, 
				NO_BLOCK_COLOR=Pg_WHITE, 
   				BLOCK_COLOR=Pg_RED, 
   				INDICATOR_ENABLED_NORMA_FILL=0x00747E92,
   				INDICATOR_ENABLED_NORMA_TEXT=0x0000FF00,
   				INDICATOR_ENABLED_NOT_NORMA_FILL=Pg_YELLOW,
   				INDICATOR_ENABLED_NOT_NORMA_TEXT=Pg_RED,
   				
   				INDICATOR_DISABLED_FILL=0x00BEBEBE,
   				INDICATOR_DISABLED_TEXT=0x00646464
				};

	system_settings_spider();

// initialization
	void prepare_visualization(ApDBase_t* new_widget_dbase);

// getters
	ApDBase_t* get_widgets_dbase() {return(widget_dbase);};

	PtWidget_t* get_main_window() {return(main_window);};
	void set_main_window(PtWidget_t *new_main_window) {main_window=new_main_window;};

	dword get_paneled_escalator_id() {return (paneled_escalator_id);}
	void set_paneled_escalator_id(dword new_paneled_escalator_id) { paneled_escalator_id=new_paneled_escalator_id; }

	PtWidget_t* get_escalators_panel() {return (escalator_panel);};
	void set_escalators_panel(PtWidget_t* new_escalator_panel) {escalator_panel=new_escalator_panel;};

	PhImage_t* get_image (images_size_type image_index);
	
	string get_start_wav_file_name() {return start_wav_file_name;};

// windows
	void message_window (const int , const string, const bool = false);		
	
// working with fonts
	bool small_font_prepared() {return (!font_for_messages_small.empty());};
	const uchar_t* get_small_font() {return this->small_font_prepared()? &font_for_messages_small[0] : NULL ;};

	bool large_font_prepared() {return (!font_for_messages_large.empty());};
	const uchar_t* get_large_font(){return this->large_font_prepared()?&font_for_messages_large[0] : NULL;};
};

#endif