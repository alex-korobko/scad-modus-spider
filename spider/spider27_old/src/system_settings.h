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
strings_container directions_text_engl;
strings_container directions_text_russ;
strings_container outer_states_text;
strings_container start_days_modes_text_engl;
strings_container start_days_modes_text_russ;

dword paneled_escalator_id;

string devices_config_name;
string global_messages_name;
string messages_types_name;
string devices_types_name;
string main_log_name;

string routing_name;

string start_wav_file_name;

public:
typedef images_container::size_type images_size_type;
typedef strings_container::size_type strings_size_type;
typedef  vector<byte>  bytes;

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

	//escalator states
	enum {OFFLINE=0,
				ONLINE=1,
				BLOCK_NORMA=1,
				BREAKING_PATH_NORMA=1
				};
				
	//escalator outer states
	enum {	DISABLED=0,
				ENABLED,
				OUTER_STATES_COUNT
				};
   
	//directions
	enum {DIRECTION_STOP=0,
				DIRECTION_UP,
				DIRECTION_DOWN,
				DIRECTION_REVERSE,
				DIRECTIONS_COUNT
				};

	//escalator start days modes
	enum {START_DAY_MODE_EVERYDAY=0,
				START_DAY_MODE_NEVER,
				START_DAY_MODE_EVEN,
				START_DAY_MODE_ODD,
				START_DAYS_MODES_COUNT,
				START_HOUR_MIN=2,
				START_HOUR_MAX=8
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
				WARN_MSG,
				MESSAGES_COUNT
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
				MODBUS_BUFFER_SIZE=100,
				TIME_CORR_INTER=10000 //interval (in cicles of requests to hardware) between sending timeset request
				};

system_settings() : 
	images(IMAGES_COUNT),
	mode_text(MODES_COUNT),
	ready_text(STATES_COUNT),
	directions_text_engl(DIRECTIONS_COUNT),
	directions_text_russ(DIRECTIONS_COUNT),
	outer_states_text(OUTER_STATES_COUNT),
	start_days_modes_text_engl(START_DAYS_MODES_COUNT),
	start_days_modes_text_russ(START_DAYS_MODES_COUNT),	
	paneled_escalator_id(0),
	devices_config_name("config.ini"),
	global_messages_name("messages.ini"),
	messages_types_name("messages_types.ini"),
	devices_types_name("devices_types.ini"),
	main_log_name("main.log"),
	routing_name("routing.ini"),
	start_wav_file_name("sounds/wp_start.wav")
	{
		widget_dbase=NULL;
		main_window=escalator_panel=NULL;

		mode_text[MODE_UNUSED]="Ошибка режима!!";
		mode_text[MODE_MAIN_DRIVE_AT_PANEL]="ГП со щита";
		mode_text[MODE_MAIN_DRIVE_AT_BEANCH_BOARD]="ГП с пульта";
		mode_text[MODE_MAIN_DRIVE_AT_REMOTE_CONTROL]="ГП с ДУ";
		mode_text[MODE_SMALL_DRIVE_AT_PANEL]="МП со щита";
		mode_text[MODE_SMALL_DRIVE_AT_BEANCH_BOARD]="МП с пульта";
		mode_text[MODE_TEST]="Тест";
		mode_text[MODE_BRAKING_OFF]="Растормаж.";
		mode_text[MODE_RUNNING_OUT]="Выбег";

		ready_text[STATE_MAIN_DRIVE_READY]="ГП готов";
		ready_text[STATE_SMALL_DRIVE_READY]="МП Готов";
		ready_text[STATE_TEST_READY]="Тест готов";
		ready_text[STATE_BRAKING_OFF_READY]="Раст. готов";
		ready_text[STATE_RUNNING_OUT_READY]="Выбег готов";
		ready_text[STATE_NOT_READY]="Не готов";

		outer_states_text[ENABLED]="enabled";
		outer_states_text[DISABLED]="disabled";

		directions_text_engl[DIRECTION_STOP]="stop";
		directions_text_engl[DIRECTION_UP]="up";
		directions_text_engl[DIRECTION_DOWN]="down";
		directions_text_engl[DIRECTION_REVERSE]="reverse";

		directions_text_russ[DIRECTION_STOP]="СТОП";
		directions_text_russ[DIRECTION_UP]="ВВЕРХ";
		directions_text_russ[DIRECTION_DOWN]="ВНИЗ";
		directions_text_russ[DIRECTION_REVERSE]="РЕВЕРС";

		start_days_modes_text_russ[START_DAY_MODE_EVERYDAY]="ежедневно";
		start_days_modes_text_russ[START_DAY_MODE_NEVER]="никгда";
		start_days_modes_text_russ[START_DAY_MODE_ODD]="нечетные";
		start_days_modes_text_russ[START_DAY_MODE_EVEN]="четные";

		start_days_modes_text_engl[START_DAY_MODE_EVERYDAY]="everyday";
		start_days_modes_text_engl[START_DAY_MODE_NEVER]="never";
		start_days_modes_text_engl[START_DAY_MODE_ODD]="odd";
		start_days_modes_text_engl[START_DAY_MODE_EVEN]="even";

	};


void prepare_visualization(ApDBase_t* new_widget_dbase) 
{
		widget_dbase=new_widget_dbase;
		
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

	strings_container get_directions_engl_strings() {return directions_text_engl;};
	strings_container get_directions_russ_strings() {return directions_text_russ;};
	strings_container get_start_days_modes_engl_strings() {return start_days_modes_text_engl;};
	strings_container get_start_days_modes_russ_strings() {return start_days_modes_text_russ;};
	strings_container get_outer_states_strings() {return outer_states_text;};

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

// splitting and merging per bytes -  very dangerous templates using
	template<typename T>
	bytes bytes_of_type (T value_to_splitting) 
				{
					bytes ret_val(sizeof(T));
					for (bytes::size_type i=0; i<ret_val.size();i++)
						{
							ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
						};
				return  ret_val;
				};

	template<typename T>
	T type_from_bytes (bytes bytes_to_merging)
				{
					T ret_val=0;
					bytes::size_type count=__min(bytes_to_merging.size(), sizeof(T));
					for (bytes::size_type i=0; i<count; i++)
						{
							ret_val |=(bytes_to_merging[i] << (i*8) );
						};
				return  ret_val;
				};

				
//	byte first_byte(int data) {return(data & 0x00FF); };
//	byte second_byte(int data)	{return((data & 0xFF00) >> 8);};

//directions managment
	int save_directions(const string file_name);
	int load_directions( const string file_name);

// working with fonts
	bool small_font_prepared() {return (!font_for_messages_small.empty());};
	const uchar_t* get_small_font() {return(&font_for_messages_small[0]);};

	bool large_font_prepared() {return (!font_for_messages_large.empty());};
	const uchar_t* get_large_font(){return(&font_for_messages_large[0] );};

//config file names in project
	string get_devices_config_name() {return(devices_config_name);};
	string get_global_messages_name() {return(global_messages_name);};
	string get_messages_types_name() {return(messages_types_name);};
	string get_devices_types_name() {return(devices_types_name);};
	string get_main_log_name() {return(main_log_name);};
	string get_routing_name() {return(routing_name);};
	
//other files names
	string get_start_wav_file_name() {return start_wav_file_name;};
};

extern system_settings g_system_settings;

#endif
