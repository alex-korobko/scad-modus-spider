/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01A */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "global.h"
#include "abimport.h"
#include "proto.h"

#include "log.h"

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
 "КАНП",//  "Ключ СТОП $$а балюстраде нижний правый                  ",  //0
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
 "ГП(п)",//  "Режим управления главным приводом с пульта             ",  //0
 "ГП(Д)",//  "Режим управления главным приводом от диспетчера        ",  //0
 "МП(щ)",//  "$$ежим управления малым приводом со щита                ",  //0
 "МП(п)",//  "Режим управления малым приводом с пульта               ",  //0
 "Тест",//  "Режим проверки контакторов и рабочего тормоза          ",  //0
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
 "ОСКВ",//  "Сигнал обратной связи от контактора верха              ",  //0
 "ОСКН",//  "Сигнал обратной связи от контактора низа               ",  //0
 "ОСКУ1",//  "Сигнал обратной связи от контактора ускорения 1        ",  //0
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
 "БТП" //  "Блок-контакт рабочего тормоза правый                    ",  //0
};

void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where)
{
	uchar_t Helvetica10[MAX_FONT_TAG];
	uchar_t font_name[] = { "Arial" };
	PtGenListItem_t *cur_item;
	PhPoint_t point;
	PhRect_t extent;
	int count, flg,strSize;
	char	str[128];
	char buffer[256];
	LogRecord* record;
   	char  translate[256*MB_LEN_MAX];	
   	int		msgID, param;
   	DictEntry*		curEntry = NULL;
	
	
	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	if (PfGenerateFontName(font_name, 0, 10, Helvetica10) == NULL)
	{
		perror("Unable to generate font name"); 
	} else	
	{    
		PgSetFont((char*)Helvetica10);
	}
	PtListColumn_t* column_pos = (PtListColumn_t*)get_widget_scalar(widget,	Pt_ARG_LIST_COLUMN_POS);


	count = 0;
	cur_item = items;
	point.x = where->ul.x;
	point.y = where->ul.y + 18;
	while(cur_item)
	{
		if ( cur_item->flags & Pt_LIST_ITEM_DAMAGED ) {
		record  = (LogRecord*)(cur_item);

		msgID = record->msgID & 0xFFFF;
		param = (record->msgID >> 16)& 0xFFFF;

		if (!(msgID & 0x8000))
		{
		curEntry = g_msgDictionary.GetByKey(msgID);
		if (!curEntry)
			return;		

		//point.x = where->ul.x + column_pos[0].from + (column_pos[0].to -  column_pos[0].from - 18)/2;
		
		// Date
		strftime(buffer, 256, "%d-%m-%Y", localtime(&record->saveTime));
		point.x = where->ul.x + 8;		



		PgSetTextColor(curEntry->color);
		PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0 );
		PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		
		// Time
		strftime(buffer, 256, "%H:%M:%S", localtime(&record->saveTime));
		
		PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0 );
		point.x = where->ul.x + column_pos[1].from + (column_pos[1].to - column_pos[1].from - extent.lr.x - extent.ul.x)/2;		
		PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		
		// Station
		point.x = where->ul.x + column_pos[2].from + COLUMN_LEFT_MARGIN;		
		if (msgID <143)
		{
			strSize = g_stations[record->stationID].Name.Get(str, 128);
			PgDrawText(str, strSize, &point, Pg_TEXT_BOTTOM);
		}


		
		// Escalator
		if (msgID <143)
		{
			itoa(record->escalatorID, buffer, 10);

			PgExtentText(&extent, NULL, (char*)Helvetica10, buffer, 0 );		
			point.x = where->ul.x + column_pos[3].from + (column_pos[3].to - column_pos[3].from - (extent.lr.x - extent.ul.x))/2;
			
			PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);
		}

		// Message
		point.x = where->ul.x + column_pos[4].from + COLUMN_LEFT_MARGIN;
			//if (translate_string(messages[msgID].text, translate, translate_set))
			//{	
				if (msgID == 119)
				{
					sprintf(buffer, "%s %d (мм)", curEntry->text, param);

				PgDrawText(buffer, strlen(buffer), &point, Pg_TEXT_BOTTOM);							
				}
				else
					{
					PgDrawText(curEntry->text, strlen(curEntry->text), &point, Pg_TEXT_BOTTOM);							
					};
			//}
		}
		else
		{

				point.x = where->ul.x + column_pos[4].from + COLUMN_LEFT_MARGIN;
				//if (translate_string(blockingNames[abs(short(msgID))], translate, translate_set))
				//{				
					PgSetTextColor(COLOR_GREEN);		
					PgDrawText(blockingNames[abs(short(msgID))], strlen(blockingNames[abs(short(msgID))]), &point, Pg_TEXT_BOTTOM);
				//}
		}
		}
		cur_item = cur_item->next;
		count++;
		point.y += 18;		
	}
}

