#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

using namespace std;

#include "definitions.h"

extern lines_container lines;
extern lines_container::size_type previos_lines_cbox_index;
extern string file_name;
extern PtWidget_t *line_choser_combobox, *main_window;

string password_of_current_line;

void save_line_parameters() throw (runtime_error);
void save_settings_parameters() throw (runtime_error);
void save() throw (runtime_error);

//static functions
static int validate_password ( void *data,
              char const *password_entered ) {
if (password_of_current_line.compare(password_entered)==0)
        return Pt_PWD_ACCEPT;

return Pt_PWD_REJECT;
};

//local functions
int activate_shutdown_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ) {
system("/usr/photon/bin/phshutdown -S system");
return Pt_CONTINUE;
};

int activate_ok_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ) {
unsigned short *current_item_index;
char const *btns[] = { "&Отмена", "&ОК"};
string message("Введите пароль для линии");

if (line_choser_combobox==NULL)
              return Pt_HALT;
PtGetResource(line_choser_combobox, Pt_ARG_CBOX_SEL_ITEM, &current_item_index, 0);
previos_lines_cbox_index=*current_item_index;

if (previos_lines_cbox_index==0 ||
    previos_lines_cbox_index>lines.size())
              return Pt_HALT;

lines_container::iterator lines_cont_iter=lines.begin();
advance(lines_cont_iter, previos_lines_cbox_index-1);

password_of_current_line=lines_cont_iter->line_password;
message+="\n";
message+=lines_cont_iter->line_name;


int result=
PtPassword(main_window,
                NULL,
                "Ввод пароля",
                NULL,
                message.c_str(),
                NULL,
                btns,
                NULL,
                NULL,
                &validate_password,
                NULL,
                NULL,
                Pt_CENTER|Pt_BLOCK_ALL);

////that in validate_password function
 if (result==Pt_PWD_ACCEPT) {
         try{ 
           save();
           cout<<"Line script "<<lines_cont_iter->line_script<<endl;
           if (execl(lines_cont_iter->line_script.c_str(), 
                     lines_cont_iter->line_script.c_str(),
                     NULL)==-1) throw runtime_error(strerror(errno));
          } catch (runtime_error run_err) {
              cout<<"In activate_ok_button "<<run_err.what()<<endl;
          }; 

     } else if (result==Pt_PWD_REJECT) {
          message="Введен неверный пароль для линии\n";
          message+=lines_cont_iter->line_name;

         PtNotice( main_window,
          NULL,
          "Пароль не верен",
           NULL,
           message.c_str(),
           NULL,
            "OK",
           NULL,
           Pt_CENTER|Pt_BLOCK_ALL);
      };


return Pt_CONTINUE;
};



