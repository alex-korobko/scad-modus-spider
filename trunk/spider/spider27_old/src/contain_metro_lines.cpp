#include "global.h"
#include "spider_exception.h"
#include "contain_morning_start.h"
#include "line.h"
#include "contain_metro_lines.h"

void metro_lines_container::set_morning_start_state_indicator( 
                  PtWidget_t *new_morning_start_state_indicator) { 
			if (morning_start_state_indicator!=NULL) 
			{
				PtSetResource(morning_start_state_indicator,
										Pt_ARG_POINTER, 
										NULL, 
										0);				
			}
	
		if (new_morning_start_state_indicator!=NULL) {
			PtSetResource(new_morning_start_state_indicator, 
									Pt_ARG_POINTER, 
									this, 
									0);				
			};		

			morning_start_state_indicator=new_morning_start_state_indicator;
};

/*
void 
  metro_lines_container::load_line_parameters()  throw (spider_exception)

That function reads line`s parameters from config file 
and insert that line into g_lines. 
	id -  id of line. If line with same id present in g_lines, 
function write error message and return false.
	name -  name of line. If not present,  
function write error message and return false.
	color  -  color of line. If not present,  
sets metro_line::DEFAULT_COLOR. Don`t set color ==0!!!
*/

void metro_lines_container::	load_line_parameters () 
       throw (spider_exception){
enum {ID=0, NAME, COLOR, ENTRIES_COUNT};
ostringstream exception_description;
string entry_name, line_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int line_id(-1);
PgColor_t line_color(0);


entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[COLOR]="color";


entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL)
{
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (this->find(temp_int)==this->end())
					{
						line_id=temp_int;
					} else 	{
                        exception_description<<"Line with id  "<<temp_int<<" already exist in lines container";
                        throw (spider_exception(exception_description.str()));
					};
			} else {
                       exception_description<<"Wrong line id  "<<&temp_str[0];
                       throw (spider_exception(exception_description.str()));
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		line_name=&temp_str[0];
	}  else if (entry_name.compare(entries_names[COLOR])==0) {
			long temp_int = strtol(&temp_str[0], NULL, 16);
			if (temp_int>0)
			{
				line_color=static_cast<PgColor_t>(temp_int);
			} else {
                       exception_description<<"Wrong line color  "<<&temp_str[0];
                       throw (spider_exception(exception_description.str()));
			};
			
	} else {
            exception_description<<"Unrecognized config entry  "<<entry_name;
            throw (spider_exception(exception_description.str()));
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size());
};


if ( line_id>0 &&
	! line_name.empty() ) 
	{
	if (line_color<=0) line_color=metro_line::DEFAULT_COLOR;
	pair_metro_lines tmp_pair (line_id,
											metro_line 
														(
														line_id,
														line_name,
														line_color
														)
											);
		
	this->set_current_line(this->insert(this->upper_bound(line_id), tmp_pair));	
	
	} else {
        exception_description<<"Not found all of required entries for line";
         throw (spider_exception(exception_description.str()));
	};

};


void 
metro_lines_container::load 
      (string file_name)  throw (spider_exception) 	{
 	enum {LINE=0, ENTRIES_COUNT};
	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";

	this->erase(begin(), end());
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
        exception_description<<"Can`t open config file "<<file_name;
        throw spider_exception(exception_description.str());
	};
	
	try {
           section_name_c_str=PxConfigNextSection();
           while (section_name_c_str!=NULL) {
               section_name=section_name_c_str;

               if (section_name.compare(sections_names[LINE])==0) {
                   load_line_parameters () ;
                } else {
                   exception_description<<"Unrecognized config section "<<section_name;
                   throw spider_exception(exception_description.str());
                };
			
               section_name_c_str=PxConfigNextSection();
           };
     } catch (spider_exception spr_exc) {
     	exception_description << spr_exc.get_description();
     	if  (PxConfigClose()!=Pt_TRUE) {
             exception_description << "\nCan`t close config file "<<file_name;
     	};
     	throw spider_exception(exception_description.str());
     };

	if  (PxConfigClose()!=Pt_TRUE) {
      exception_description << "Can`t close config file "<<file_name;
      throw spider_exception(exception_description.str());
	};
}; 

