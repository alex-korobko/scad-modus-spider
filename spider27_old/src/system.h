#ifndef __SYSTEM_H__
#define __SYSTEM_H__

class system_settings
{
private:
typedef vector<PhImage_t> images_container;
typedef vector<string> strings_container;
typedef vector<uchar_t> unsigned_chars_container;

ApDBase_t *widget_dbase;
PtWidget_t *main_window, *escalator_panel;
	
unsigned_chars_container font_for_messages_small;
unsigned_chars_container font_for_messages_large;

images_container images;
strings_container mode_text;
strings_container ready_text;

dword paneled_escalator_id;

string devices_config_name;
string global_messages_name;
string escalator_types_name;
string main_log_name;

string routing_name;

public:
typedef images_container::size_type images_size_type;
typedef strings_container::size_type strings_size_type;

//Enumerated constants

	// PtGenLists and childs parameters
	enum{
				COLUMN_LEFT_MARGIN =5,
				ROW_HEIGHT=18
			};

	// indlcator`s images
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
   				IMAGES_COUNT
   				};

	//states
	enum {OFFLINE=0,
				ONLINE=1,
				DISABLED=0,
				ENABLED=1,
				BLOCK_NORMA=1,
				BREAKING_PATH_NORMA=1
				};
   
	//directions
	enum {DIRECTION_STOP=0,
				DIRECTION_UP,
				DIRECTION_DOWN,
				DIRECTION_REVERSE
				};

	//escalator command codes (for modbus register 40001)
	enum {COMMAND_UP=0xE0,
				COMMAND_DOWN=0xE1,
				COMMAND_STOP=0xE2
				};

	//escalator states from variable Condition (modbus register 30001) 
	enum {STATE_UP_MAIN_DRIVE=2,
				STATE_DOWN_MAIN_DRIVE,
				STATE_UP_SMALL_DRIVE,
				STATE_DOWN_SMALL_DRIVE,
				STATE_STOP
				};

	//escalator`s modes codes  from variable Mode (modbus register 30002) 
	enum {MODE_UNUSED=0,
				MODE_MAIN_DRIVE_AT_PANEL, 
				MODE_MAIN_DRIVE_AT_BEANCH_BOARD, 
				MODE_MAIN_DRIVE_AT_REMOTE_CONTROL,
				MODE_SMALL_DRIVE_AT_PANEL,
				MODE_SMALL_DRIVE_AT_BEANCH_BOARD,
				MODE_TEST,
				MODE_BRAKING_OFF, 
				MODE_RUNNING_OUT, 
				MODES_COUNT};
	
	//escalator`s  ready states codes from variable Gotov (modbus register 30003) 
	enum {STATE_MAIN_DRIVE_READY=0,
				STATE_SMALL_DRIVE_READY,
				STATE_TEST_READY,
				STATE_BRAKING_OFF_READY,
				STATE_RUNNING_OUT_READY,
				STATE_NOT_READY,
				STATES_COUNT
				};

// message types
	enum{
				DEBUG_MSG=0,
				ERROR_MSG,
				INFO_MSG,
				WARN_MSG
			};

//colors 
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

// other system settings
	enum {
				SYSTEM_TIMER=_PULSE_CODE_MINAVAIL,
				PING_TIMEOUT=2000000, // мкс
				SYSTEM_TICK	=1, // c
				RECEIVE_TIMEOUT=1,
				MODBUS_TCP_PORT=502,
				TIME_CORR_INTER=10000 //interval (in cicles of requests to hardware) between sending timeset request
				};

system_settings() : 
	paneled_escalator_id(0),
	devices_config_name("config.ini"),
	global_messages_name("messages.ini"),
	escalator_types_name("escalator_types.ini"),
	main_log_name("main.log"),
	routing_name("routing.ini")
	{
		widget_dbase=NULL;
		main_window=escalator_panel=NULL;
	
		mode_text.resize(MODES_COUNT);
		mode_text[MODE_UNUSED]="Ошибка режима!!";
		mode_text[MODE_MAIN_DRIVE_AT_PANEL]="ГП со щита";
		mode_text[MODE_MAIN_DRIVE_AT_BEANCH_BOARD]="ГП с пульта";
		mode_text[MODE_MAIN_DRIVE_AT_REMOTE_CONTROL]="ГП с ДУ";
		mode_text[MODE_SMALL_DRIVE_AT_PANEL]="МП со щита";
		mode_text[MODE_SMALL_DRIVE_AT_BEANCH_BOARD]="МП с пульта";
		mode_text[MODE_TEST]="Тест";
		mode_text[MODE_BRAKING_OFF]="Растормаж.";
		mode_text[MODE_RUNNING_OUT]="Выбег";

		ready_text.resize(STATES_COUNT);
		ready_text[STATE_MAIN_DRIVE_READY]="ГП готов";
		ready_text[STATE_SMALL_DRIVE_READY]="МП Готов";
		ready_text[STATE_TEST_READY]="Тест готов";
		ready_text[STATE_BRAKING_OFF_READY]="Раст. готов";
		ready_text[STATE_RUNNING_OUT_READY]="Выбег готов";
		ready_text[STATE_NOT_READY]="Не готов";
	
	};


void prepare_visualization(ApDBase_t* new_widget_dbase) 
{
		widget_dbase=new_widget_dbase;
		
		images.resize(IMAGES_COUNT);		
		images[GREEN_UP]=*ApGetImageRes(widget_dbase, "green_up");
		images[GREEN_DOWN]=*ApGetImageRes(widget_dbase, "green_down");	
		images[RED_UP]=*ApGetImageRes(widget_dbase, "red_up");
		images[RED_DOWN]=*ApGetImageRes(widget_dbase, "red_down");
		images[YELLOW_UP]=*ApGetImageRes(widget_dbase, "yellow_up");
		images[YELLOW_DOWN]=*ApGetImageRes(widget_dbase, "yellow_down");
		images[GREEN_S_UP]=*ApGetImageRes(widget_dbase, "green_s_up");
		images[GREEN_S_DOWN]=*ApGetImageRes(widget_dbase, "green_s_down");
		images[RED_S_UP]=*ApGetImageRes(widget_dbase, "red_s_up");		
		images[RED_S_DOWN]=*ApGetImageRes(widget_dbase, "red_s_down");	
		images[YELLOW_S_UP]=*ApGetImageRes(widget_dbase, "yellow_s_up");
		images[YELLOW_S_DOWN]=*ApGetImageRes(widget_dbase, "yellow_s_down");
		images[GREY_STOP]=*ApGetImageRes(widget_dbase, "grey_stop");		
		images[GREY_OFF]=*ApGetImageRes(widget_dbase, "grey_off");		
		images[BLOCK_GREEN_LED]=*ApGetImageRes(widget_dbase, "block_green_led");		
		images[BLOCK_RED_LED]=*ApGetImageRes(widget_dbase, "block_red_led");		
		images[BLOCK_GREY_LED]=*ApGetImageRes(widget_dbase, "block_grey_led");		
		images[BLOCK_BLUE_LED]=*ApGetImageRes(widget_dbase, "block_blue_led");								
		
		font_for_messages_large.resize(MAX_FONT_TAG);
		string font_name("Helvetica");
		int  font_size(12);

		if (PfGenerateFontName ( (const uchar_t*) font_name.c_str(),
												PF_STYLE_ANTIALIAS | PF_STYLE_BOLD,
												font_size,
												&font_for_messages_large[0]) ==NULL) 
												{
												font_for_messages_large.erase(font_for_messages_large.begin(), font_for_messages_large.end());

												vector<char> tmp_buff(10);
												string mess ="ERROR: Can`t generate font name for font ";
												mess+=font_name;
												mess+=itoa(font_size, &tmp_buff[0], 10);
												vslogf(_SLOG_SETCODE(_SLOGC_SPIDER, 0), _SLOG_ERROR, mess.c_str(), NULL);
												};

		font_for_messages_small.resize(MAX_FONT_TAG);		
		font_name="Arial";
		font_size=10;
		
		if (PfGenerateFontName ((const uchar_t*) font_name.c_str(),
												PF_STYLE_ANTIALIAS,
												font_size,
												&font_for_messages_small[0]) ==NULL) 
												{
												font_for_messages_small.erase(font_for_messages_small.begin(), font_for_messages_small.end());						

												vector<char> tmp_buff(10);
												string mess ="ERROR: Can`t generate font name for font ";
												mess+=font_name;
												mess+=itoa(font_size, &tmp_buff[0], 10);
												vslogf(_SLOG_SETCODE(_SLOGC_SPIDER, 0), _SLOG_ERROR, mess.c_str(), NULL);

												};

}

//get_ and set_ metods for private data members
	ApDBase_t* get_widgets_dbase() {return(widget_dbase);};

	PtWidget_t* get_main_window() {return(main_window);};
	void set_main_window(PtWidget_t *new_main_window) {main_window=new_main_window;};

	dword get_paneled_escalator_id() {return (paneled_escalator_id);}
	void set_paneled_escalator_id(dword new_paneled_escalator_id) { paneled_escalator_id=new_paneled_escalator_id; }

	PtWidget_t* get_escalators_panel() {return (escalator_panel);};
	void set_escalators_panel(PtWidget_t* new_escalator_panel) {escalator_panel=new_escalator_panel;};


	PhImage_t* get_image (system_settings::images_size_type image_index) 
		{
		if (image_index>=0 && 
			image_index<images.size()) 
			{
				return(&images[image_index]);
			} else {
				return(&images[GREY_OFF]); //what can I do???
			};
		};
		
	string get_escalator_mode_text(system_settings::strings_size_type mode_index)
		{
		if (mode_index>=0 &&
			mode_index<mode_text.size()) 
			{
				return(mode_text[mode_index]);
			} else {
				return(mode_text[MODE_UNUSED]); //what can I do???
			};

		};
		
	string get_escalator_ready_text(system_settings::strings_size_type ready_index)
	{
		if (ready_index>=0 &&
			ready_index<ready_text.size()) 
			{
				return(ready_text[ready_index]);
			} else {
				return(ready_text[STATE_NOT_READY]); //what can I do???
			};
	}

//system messages
	void sys_message (const int type, const string mess_text);
	void message_window (const int type, const string mess_text);

//CRC16 coding function
	word crc(vector<byte> buffer);
	byte first_byte(int data) {return(data & 0x00FF); };
	byte second_byte(int data)	{return((data & 0xFF00) >> 8);};

//directions managment
	int save_directions(const string file_name);
	int load_directions( const string file_name);

// working with fonts
	bool small_font_prepared() {return (!font_for_messages_small.empty());};
	const uchar_t* small_font() {return(&font_for_messages_small[0]);};

	bool large_font_prepared() {return (!font_for_messages_large.empty());};
	const uchar_t* large_font(){return(&font_for_messages_large[0] );};

//config file names in project
	string get_devices_config_name() {return(devices_config_name);};
	string get_global_messages_name() {return(global_messages_name);};
	string get_escalator_types_name() {return(escalator_types_name);};
	string get_main_log_name() {return(main_log_name);};
	string get_routing_name() {return(routing_name);};

};

#endif
