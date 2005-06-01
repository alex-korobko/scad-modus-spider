using namespace std;

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>

#include <Ap.h>
#include <Pt.h>

#include "defines.h"
#include "system_settings.h"
#include "system_settings_spider.h"

	system_settings_spider::system_settings_spider() : 
	system_settings(),
	images(IMAGES_COUNT),
	paneled_escalator_id(0),
	start_wav_file_name("sounds/wp_start.wav") {
	 	widget_dbase=NULL;
		main_window=escalator_panel=NULL;
	};


void system_settings_spider::prepare_visualization(ApDBase_t* new_widget_dbase) 
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
		images[MSG_ERROR]=*ApGetImageRes(widget_dbase, "errorMsg");
		images[MSG_INFO]=*ApGetImageRes(widget_dbase, "infoMsg");
		images[MSG_WARN]=*ApGetImageRes(widget_dbase, "warnMsg");

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

PhImage_t* system_settings_spider::get_image (images_size_type image_index)
		{
		if (image_index>=0 && 
			image_index<images.size()) 
			{
				return(&images[image_index]);
			} else {
				return(&images[GREY_OFF]); //what can I do???
			};
		};

void system_settings_spider::message_window (const int type, const string mess_text, const bool is_detached=false)
{
PhImage_t *window_icon;
string window_title;


	switch(type)
	{
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

	if (!font_for_messages_small.empty() || !font_for_messages_large.empty() )
	{
	is_detached ?
	PtNotice(NULL, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					(const char*) &font_for_messages_small[0], 
					"Ok", 
					(const char*) &font_for_messages_large[0],
					Pt_ESC_DISABLE | Pt_RIGHT) 
					 : 
	PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					(const char*) &font_for_messages_small[0], 
					"Ok", 
					(const char*) &font_for_messages_large[0],
					Pt_ESC_DISABLE | Pt_MODAL | Pt_CENTER) ;
	} else {
			string mess ="ERROR: Can`t found small or large font for message window";
			vslogf(_SLOG_SETCODE(_SLOGC_SPIDER, 0), _SLOG_ERROR, mess.c_str(), NULL);


	is_detached ?
		PtNotice(NULL, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					NULL, 
					"Ok", 
					NULL,
					Pt_ESC_DISABLE | Pt_RIGHT)
					 : 
		PtNotice(main_window, 
					NULL, 
					window_title.c_str(),
					window_icon, 
					mess_text.c_str(), 
					NULL, 
					"Ok", 
					NULL,
					Pt_ESC_DISABLE | Pt_MODAL | Pt_CENTER);

	}; 
};

