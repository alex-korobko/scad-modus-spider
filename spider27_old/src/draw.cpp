/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

/*
void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        ) {
	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	}


void draw_command_pool_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        ) {
	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	}

*/

char *blockingNames[] = {
 "",//  "Блок-контакт рабочего тормоза правый                    ",  //0
 "БТЛ",//  "Блок-контакт рабочего тормоза левый                     ",  //0
 "БСН",//  "Блок-контакт ступени нижний                            ",  //0
 "БСВ",//  "Блок-контакт ступени верхний                           ",  //0
 "БЗП",//  "Блок-контакт натяжных звездочек правый                 ",  //0
 "БЗЛ",//  "Блок-контакт натяжных звездочек левый                  ",  //0
 "БМ",//  "Блок-контакт малого привода                            ",  //0
 "ТС",//  "Кнопка отмены сигнала СТОП от ключей                   ",  //0
 "БПП",//  "Блок-контакт натяжения поручня правый                  ",  //0
 "БПЛ",//  "Блок-контакт натяжения поручня левый                   ",  //0
 "БОП",//  "Блок-контакт остановки тросом правый                   ",  //0
 "БОЛ",//  "Блок-контакт остановки тросом левый                    ",  //0
 "КЗН+КСН",//  "Кнопка СТОП и ключ запрета нижний                      ",  //0
 "КАНЛ",//  "Ключ СТОП на балюстраде нижний левый                   ",  //0
 "КАНП",//  "Клю СТОП $$а балюстраде нижний праый                  ",  //0
 "КАНК",//  "Ключ СТОП контролера нижний                            ",  //0
 "КЗВ+КСВ",//  "Кнопка СТОП и ключ запрета верхний                     ",  //0
 "КАВЛ",//  "Ключ СТОП на балюстраде верхний левый                  ",  //0
 "КАВП",//  "Ключ СТОП на балюстраде верхний правый                 ",  //0
 "КАВК",//  "Ключ СТОП контролера верхний                           ",  //0
 "ТР",//  "Температурное реле                                     ",  //0
 "БР",//  "Блок-контакт рубильника ввода                          ",  //0
 "БГЛ",//  "Блок-контакт гайки АТ левый                            ",  //0
 "БГП",//  "Блок-контакт гайки АТ правый                           ",  //0
 "БАЛ",//  "Блок-контакт аварийного тормоза левый                  ",  //0
 "БАП",//  "Блок-контакт аварийного тормоза правый                 ",  //0
 "QS1",//  "Рубильник главного привода                             ",  //0
 "RU24V",//  "Сопротивление утечки управляющего напряжения +24В      ",  //0
 "QF2",//  "Автомат малого привода                                 ",  //0
 "QF3",//  "Автомат рабочего тормоза                               ",  //0
 "ГП(щ)",// "Режим управления главным приводом со щита              ",  //0
 "ГП(п)",//  "Режим управления главным прводом с пульта             ",  //0
 "ГП(Д)",//  "Режм управления глвным приводм от диспетчера        ",  //0
 "МП()",//  "Режим управления малым приодом со щита                ",  //0
 "МП(п)",//  "Режим управления малым приводом с пульта               ",  //0
 "Тет",//  "Режим проверки контакторов и рабочего тормоза          ",  //0
 "Раст.",//  "Режим растормаживания аварийного тормоза               ",  //0
 "Выбег",//  "Режим измерения выбега                                 ",  //0
 "ВЕРХщ",//  "Кнопка ВВЕРХ на щите                                   ",  //0
 "ВНИЗщ",//  "Кнопка ВНИЗ на щите                                    ",  //0
 "СТОПщ",//  "Кнопка СТОП на щите                                    ",  //0
 "ГПВп",//  "Кнопка пуска главного привода ВВЕРХ на пультах         ",  //0
 "ГПНп",//  "Кнопка пуска главного привода ВНИЗ на пультах          ",  //0
 "МПВп",//  "Кнопка пуска малого привода ВВЕРХ на пультах           ",  //0
 "МПНп",//  "Кнопка пуска малого привода ВНИЗ на пультах            ",  //0
 "ДИМ0",//  "Сигнал от датчика ДИМ0                                 ",  //0
 "ДИМ90",//  "Сигнал от датчика ДИМ90                                ",  //0
 "ДИМ0П",//  "Сигнал от датчика ДИМ0П                                ",  //0
 "ОСТР",// "Сигнал обратной связи от токового реле                  ",  //0
 "ОСКД",//  "Сигнал обратной связи от контактора дублирующего       ",  //0
 "ОСКВ",//  "Сигнал обратной связи от контктора верха              ",  //0
 "ОСКН",//  "Сигнал обратной свз от контактора низа               ",  //0
 "ОСКУ1",//  "Синал братной связ от контактора ускорения 1        ",  //0
 "ОСКУ2",//  "Сигнал обратной связи от контактора ускорения 2        ",  //0
 "ОСКУ3",//  "Сигнал обратной связи от контактора ускорения 3        ",  //0
 "ОСКУ4",//  "Сигнал обратной связи от контактора ускорения 4        ",  //0
 "ОСЭАТ",//  "Исправность катушки электромагнита аварийного тормоза  ",  //0
 "ОСКТ",//  "Сигнал обратной связи от контактора тормоза            ",  //0
 "ОСДП",//  "Сигнал открытия передней двери шкафа управления        ",  //0
 "ОСДЗ",//  "Сигнал открытия задней двери шкафа управления          ",  //0
 "+24В",//  "Сигнал наличия напряжения управления +24В              ",  //0
 "БВВ",//  "Блок-контакт входной площадки верхней                  ",  //0
 "БВН",//  "Блок-контакт входной площадки нижней                   ",  //0
 "ДПС",//
 "ВПНЛ",//
 "ВПНП",//
 "ВПВЛ",//
 "ВПВП",//
 "БКП",//Блок-контакт проема
 "РКН",// "Реле контроля напряжения  фазы                          ",  //0
 "КСР",//"Контроль стыковки разъема датчика                        ",  //0
 "Стенд",// "Контроль подключения стенда                             ",  //0
 "",
 "",
 "",
 "БТП" //  "Блок-контакт абочего тормоза правый                    ",  //0
};

/*
drawing main log items
*/

void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where)
{
	PtListColumn_t* column_pos;

    	int		msgID, param;

	PtGenListItem_t *cur_item;
	PhPoint_t point;
	unsigned int count;
	
	log_records_container *log_rec_container;
	log_records_container::iterator_log_rec iter_log_rec;	
	
	vector<PtArg_t> args;
	args.resize(2);	
	PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &column_pos, 0);
	PtSetArg(&args[1], Pt_ARG_POINTER, &log_rec_container, 0);
	PtGetResources(widget, args.size(), &args[0]);

	if (log_rec_container==NULL) 
		{
		string mess="Can`t get pointer to log_records_container class for driving log records";
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return;
		};
	
	PtGenListDrawBackground(widget, items, nitems, where, 0, 0, 0, 0 );
	
	if (! g_system_settings.small_font_prepared())
			{
			string mess="Can`t get small font from g_system_settings class for driwing log records";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);

			PgSetFont((char *) (g_system_settings.small_font()) );
			};

	cur_item = PtGenListFirstItem(widget);
	iter_log_rec=log_rec_container->begin();

	//moving to item index - nothing to do
	count=1;
	while (count < index && cur_item && iter_log_rec!=log_rec_container->end())
	{
	iter_log_rec++;
	cur_item=cur_item->next;
	count++;
	}
	
	point.x = where->ul.x;
	point.y = where->ul.y + system_settings::ROW_HEIGHT;
	//moving from item index - draw nitems items
	while(count<=nitems && iter_log_rec!=log_rec_container->end() && cur_item)
	{
		string str;
		str.reserve(128);

		vector<char> tmp_buffer(128);

		msgID = iter_log_rec->get_msg_id() & 0xFFFF;
		param = (iter_log_rec->get_msg_id() >> 16)& 0xFFFF;
		
		if (!(msgID & 0x8000))
		{
			//Date
			time_t tmp_time=iter_log_rec->get_record_time();
			strftime(&tmp_buffer[0], tmp_buffer.size(), "%d-%m-%Y", localtime(&tmp_time));
			str=&tmp_buffer[0];
			point.x = where->ul.x + column_pos[0].from + system_settings::COLUMN_LEFT_MARGIN;		
			PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		
			//Time
			str="";
		
			point.x = where->ul.x + column_pos[1].from + system_settings::COLUMN_LEFT_MARGIN;				
			PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		
			//Station
			str="";
		
			point.x = where->ul.x + column_pos[2].from + system_settings::COLUMN_LEFT_MARGIN;		
			PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		
			// Escalator number
			str="";
		
			point.x = where->ul.x + column_pos[3].from + system_settings::COLUMN_LEFT_MARGIN;		
			PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		
			//Message text
			str="";
		
			point.x = where->ul.x + column_pos[3].from + system_settings::COLUMN_LEFT_MARGIN;		
			PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		} else{

		};
		point.y = point.y + system_settings::ROW_HEIGHT;
		cur_item = cur_item->next;		
		log_rec_container++;
		count++;
	}

/*
	uchar_t Helvetica10[MAX_FONT_TAG];
	uchar_t font_name[] = { "Arial" };
	PtGenListItem_t *cur_item;
	PhPoint_t point;
	PhRect_t extent;
	PtListColumn_t* column_pos;
	
	int count;
	char buffer[256];
	log_record *record;
    	int		msgID, param;
   	msg_dict_container::msg_dict_iterator p_dict_entry;	
 	metro_escalators_container::iterator_metro_escalators p_esc_entry;
  	metro_stations_container::iterator_metro_stations p_stat_entry;
  	
  //for block circuts
  	char pblock_text[256];
	int typeIndex;
	
	
	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	if (PfGenerateFontName(font_name, 0, 10, Helvetica10) == NULL)
	{
		perror("Unable to generate font name"); 
	} else	
	{    
		PgSetFont((char*)Helvetica10);
	}
// 	PtListColumn_t* column_pos = (PtListColumn_t*)get_widget_scalar(widget,	Pt_ARG_LIST_COLUMN_POS);

	PtGetResource(widget,	Pt_ARG_LIST_COLUMN_POS,  column_pos, 0);


	count = 0;
	cur_item = items;
	point.x = where->ul.x;
	point.y = where->ul.y + 18;
	while(cur_item)
	{
		if ( cur_item->flags & Pt_LIST_ITEM_DAMAGED ) {
		record  = (log_record*)(cur_item);

		msgID = record->get_msg_id() & 0xFFFF;
		param = (record->get_msg_id() >> 16)& 0xFFFF;

		if (!(msgID & 0x8000))
		{
		p_dict_entry=g_msgDictionary.find(msgID);

		if(p_dict_entry==g_msgDictionary.end())
			return;		

		//point.x = where->ul.x + column_pos[0].from + (column_pos[0].to -  column_pos[0].from - 18)/2;
		
		// Date
		time_t tmp_time
		strftime(buffer, 256, "%d-%m-%Y", localtime(&));
		point.x = where->ul.x + 8;		

//		PgSetTextColor(curEntry->color);
		PgSetTextColor((*p_dict_entry).second.color);
		PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0);
		PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		
		// Time
		strftime(buffer, 256, "%H:%M:%S", localtime(&record->get_save_time()));
		
		PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0 );
		point.x = where->ul.x + column_pos[1].from + (column_pos[1].to - column_pos[1].from - extent.lr.x - extent.ul.x)/2;		
		PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		
		// Station
		point.x = where->ul.x + column_pos[2].from + system_settings::COLUMN_LEFT_MARGIN;		

		if (msgID <143)
		{
			p_stat_entry=g_stations.find(record->get_station_id());
			if (p_stat_entry!=g_stations.end())			
			 PgDrawText(p_stat_entry->second.get_c_name_string(), p_stat_entry->second.get_c_name_string_size(), &point, Pg_TEXT_BOTTOM);
		}
		
		// Escalator
		if (msgID <143)
		{
			itoa(record->get_escalato_id(), buffer, 10);

			PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0 );		
			point.x = where->ul.x + column_pos[3].from + (column_pos[3].to - column_pos[3].from - (extent.lr.x - extent.ul.x))/2;
			
			PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		}

		// Message
		point.x = where->ul.x + column_pos[4].from + system_settings::COLUMN_LEFT_MARGIN;
		if (msgID == 119)
				{
					sprintf(buffer, "%s %d (мм)", (*p_dict_entry).second.text, param);

				PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);							
				}
				else
					{
					PgDrawText((*p_dict_entry).second.text, strlen((*p_dict_entry).second.text), &point, Pg_TEXT_BOTTOM);							
					};
		}
		else
		{

				point.x = where->ul.x + column_pos[4].from + system_settings::COLUMN_LEFT_MARGIN;
					PgSetTextColor(system_settings::COLOR_GREEN);		
	
		p_esc_entry=g_escalators.find(record->get_escalator_id());
		if (p_esc_entry!=g_escalators.end() && p_esc_entry->second.get_station_id() == record->get_station_id())
		{
					if (g_escTypes)
						{
						typeIndex = p_esc_entry->second.get_type();

						if (typeIndex!=0) {
														
//						printf("\nEscalator type %d id %d message %d record ID %d record station %d mess id %d", typeIndex, g_escalators[i].id, msgID, record->escalatorID, record->stationID, abs(short(msgID)));

						string tmp_string=g_escTypes[typeIndex].blocks[0].signals[abs(short(msgID))].name;			
						PgDrawText(tmp_string.c_str(), tmp_string.size() , &point, Pg_TEXT_BOTTOM);
														} else {								
 					    PgDrawText(blockingNames[abs(short(msgID))], strlen(blockingNames[abs(short(msgID))]), &point, Pg_TEXT_BOTTOM);
														};
						}
		};	
	
	
				//}
		}
		}
		cur_item = cur_item->next;
		count++;
		point.y += 18;		
	}
*/
}

void draw_command_pool_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where)
{
	PtListColumn_t* column_pos;
	PtGenListItem_t *cur_item;
	PhPoint_t point;
	unsigned int count;
	string str;

	cmd_pool_container	*p_cmd_pool_container;
	cmd_pool_container::cmd_pool_iterator p_cmd_pool;	
  	metro_stations_container::iterator_metro_stations p_stat_entry;
	
	vector<PtArg_t> args;
	args.resize(2);	
	PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &column_pos, 0);
	PtSetArg(&args[1], Pt_ARG_POINTER, &p_cmd_pool_container, 0);
	PtGetResources(widget, args.size(), &args[0]);

	if (p_cmd_pool_container==NULL) 
		{
		string mess="Can`t get pointer to cmd_pool_container class for driving command pool records";
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return;
		};
	
	PtGenListDrawBackground(widget, items, nitems, where, 0, 0, 0, 0 );
	
	if (!g_system_settings.small_font_prepared())
			{
			string mess="Can`t get small font from g_system_settings for driving command pool records";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);

			PgSetFont((char *) (g_system_settings.small_font()) );
			};


	cur_item = PtGenListFirstItem(widget);
	p_cmd_pool=g_CommandPool.begin();

	//moving to item index - nothing to do
	count=1;
	while (count < index && cur_item &&p_cmd_pool!=g_CommandPool.end())
	{
	p_cmd_pool++;
	cur_item=cur_item->next;
	count++;
	}
	
	point.x = where->ul.x;
	point.y = where->ul.y + system_settings::ROW_HEIGHT;
	//moving from item index - draw nitems items
	while(count<=nitems && p_cmd_pool!=g_CommandPool.end() && cur_item)
	{
		PgSetTextColor(p_cmd_pool->get_item_color());

		point.x = where->ul.x + column_pos[0].from + system_settings::COLUMN_LEFT_MARGIN;		
		
		// Station
		p_stat_entry=g_stations.find(p_cmd_pool->get_station());
		if(p_stat_entry!=g_stations.end())
				PgDrawText(p_stat_entry->second.get_c_name_string(), p_stat_entry->second.get_c_name_string_size(), &point, Pg_TEXT_BOTTOM);
				
		// Escalator
		str=itoa(p_cmd_pool->get_escalator_number(), NULL, 10);				
		point.x = where->ul.x + column_pos[1].from + system_settings::COLUMN_LEFT_MARGIN;		
		PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);

		//Command text
		if (p_cmd_pool->get_command_code()==system_settings::COMMAND_UP) 
		{
			str="BBEPХ";				
		} else	if (p_cmd_pool->get_command_code()==system_settings::COMMAND_DOWN) {
			str="НИЗ";				
		} else
			str="ОИБКА!!";				 
		
	
		point.x = where->ul.x + column_pos[2].from + system_settings::COLUMN_LEFT_MARGIN;		
		PgDrawText(str.c_str(), str.size(), &point, Pg_TEXT_BOTTOM);
		
		point.y = point.y + system_settings::ROW_HEIGHT;
		cur_item = cur_item->next;		
		p_cmd_pool++;
		count++;
	}
	
}

