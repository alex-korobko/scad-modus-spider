#ifndef _UDKU_DATA_BLOCK_
#define _UDKU_DATA_BLOCK_

/*
Fields of struct udku_data_block;

status : 
	modbus register 30001 
     high byte
        not used
     low byte
        code mean escalator state (see documents)
        0-GPSTOP
        1-GPUP
        2-GPDOWN
        3-UP20
        4-UP25
        5-NOT7SEC
        6-BREAK
        7-SAMOHOD
        8-BRAK0
        9-BRAK90
        10-BRAKALLES
        11-BRAKKSR

	modbus register 30002
     high byte
        DKSE position
            0 in left
            1 in right
     low byte
        escalator type & speed (m/s)
		udku_type_text[UDKU_TYPE_LT_1]="ЛТ-1 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_2_1]="ЛТ-2 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_2_2]="ЛТ-2 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_3_1]="ЛТ-3 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_3_2]="ЛТ-3 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_3M_1]="ЛТ-3М 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_3M_2]="ЛТ-3М 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_4_1]="ЛТ-4 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_4_2]="ЛТ-4 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_5_1]="ЛТ-5 0.94 м/с";
		udku_type_text[UDKU_TYPE_LT_5_2]="ЛТ-5 0.94 м/с";
		udku_type_text[UDKU_TYPE_ET_2]="ЭТ-2 0.94 м/с";
		udku_type_text[UDKU_TYPE_ET_3_1]="ЭТ-3 0.94 м/с";
		udku_type_text[UDKU_TYPE_ET_3_2]="ЭТ-3 0.94 м/с";
		udku_type_text[UDKU_TYPE_ET_3M_1]="ЭТ-3М 0.94 м/с";	
		udku_type_text[UDKU_TYPE_ET_5]="ЭТ-5 0.94 м/с";
		udku_type_text[UDKU_TYPE_ET_5M_1]="ЭТ-5М 0.94 м/с";
		udku_type_text[UDKU_TYPE_EM_4]="ЭМ-4 0.94 м/с";
		udku_type_text[UDKU_TYPE_EM_5_5]="ЭМ-5.5 0.94 м/с";

	modbus register 30003
     high byte
        Is breaking path value normal?
            0 NOT normal
            0xff normal
     low byte
          third byte of breaking path value

     modbus register 30004
     high byte
          second byte of breaking path value
     low byte
          first byte of breaking path value

     modbus register 30005
     high byte
          nominal escalator speed value, centimeters per second
     low byte
          current escalator speed value, centimeters per second

	modbus register 30006
     high byte
          fourth byte of escalator running path (porbeg, kilometers)
     low byte
          third byte of escalator running path (porbeg, kilometers)

	modbus register 30007
     high byte
          second byte of escalator running path (porbeg, kilometers)
     low byte
          first byte of escalator running path (porbeg, kilometers)

	modbus register 30008 
     high byte
        bit number (1 active state)
        7 КАНЛ+РДО+КАНП (Ключі «СТОП» на балюстраді нижні + вимикач СУРСТ)
        6 ККВ+КАВЛ+КАВП (Ключі «СТОП» на балюстраді верхні)
        5 КнС2+КЗП2 (Ключі заборони пуску верхні)
        4 ДУО (Дистанційна зупинка)
        3 Not used
        2 Mashzal door (1 means door is open)
        1 РКН2 (Реле контролю напруги +110В)
        0 РКН1 (Реле контролю напруги ~380В)
     low byte
        bit number (1 active state)
        7 RKP rele of  start control (0 means escalator is running on nominal speed, 1 mean escalator is stopped or in starting mode)
        6 RB2 circut of power supply (1 mean is power supply NOT norma)
        5 KT breaks kontaktor (1 mean breaks is ON)
        4 RG Готовність телемеханіки (1 means on TU mode)
        3 RPV1 kontactor  UP (1 means escalator running UP)
        2 RPN1 kontactor DOWN (1 means escalator running DOWN)
        1 RBK (Реле блокувального кола 0 means it is good, 1 means one  of block circuts is broken)
        0 B16 (Вимикач допоміжного приводу)

	modbus register 30009 (UDKU inputs - block circut contacts)
     high byte
        bit number (1 active state)
        7 РТЛ+РТП (Термореле підшипника вхідного валу редуктора)
        6 РУП+РУЛ (Контакти реле заклинювання поручнів)
        5 ВПС (Блокування підйому ступені нижнє)
        4 ВВНП+ВВНЛ (Блокування нижньої вхідної площадки)
        3 ВНП+ВСПН+ВСЛН+ВНЛ (Блокування натяжного пристрою + опускання ступені нижнє)
        2 ВПНЛ+ВПНП (Блокування сходу поручня нижнє)  
        1 ВПП+ВПЛ+ВПВЛ+ВПВП (Блокування витяжки + сходу поручня верхнє)
        0 ВБП+ВБЛ (Блокування бігунка ступені)

     low byte
        7 ВСПВ+ВСЛВ (Блокування опускання ступені верхнє)
        6 ВВС+ВОП+ВОЛ (Блокування демонтажу ступені + блокування «СТОП» в нахилі)
        5 ВВВП+ВВВЛ (Блокування верхньої вхідної площадки)
        4 ВВ+ВА+ВГ (Гвинт + упор + гайка аварійного гальма)
        3 РЗ+РАТ (Блокування зі схеми аварійного гальма)
        2 ВТ (Блокування робочого гальма)
        1 КнС (Кнопка «СТОП» на шафі керування)
        0 КнС3+КЗП3 (Ключі заборони пуску нижні)

	modbus register 30009
      upper message id in messages pull

	modbus register 30010
      messages count in messages pull

	modbus register form 30011 to 30011+COUNT_OF_MESSAGES
     messages pull
*/


struct udku_data_block : public data_block{
	enum {
        MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS=30001,
        MODBUS_DIRECTION_REGISTERS_BEGIN_ADDRESS=40001,

        MODBUS_LOW_INPUT_ADDRESS=30001,
        MODBUS_HIGH_INPUT_ADDRESS=30011+system_settings::MAX_MESSAGES_COUNT,
        MODBUS_LOW_DIRECTION_ADDRESS=40001,
        MODBUS_HIGH_DIRECTION_ADDRESS=40003};

	enum {SIGNAL_VALUE_GREEN=0,
                SIGNAL_VALUE_BLUE=1,
                SIGNAL_VALUE_RED=2,
	            SIGNAL_VALUE_GREY=3};

	enum {SIGNALS_COUNT=34,
                PARAMETERS_COUNT=8,
                MESSAGES_COUNT=16};

	enum {ALERT_ERROR_MESSAGES_TYPE=2};

	enum {INDEX_SIGNAL_BREAKING_PATH_STATUS=1,
   INDEX_SIGNAL_MASHZAL_DOOR=28,
   INDEX_SIGNAL_RKN1=26,
   INDEX_SIGNAL_RKN2=27,
   INDEX_SIGNAL_RKP=25,
   INDEX_SIGNAL_CONTACTOR_RV2=24,
   INDEX_SIGNAL_CONTACTOR_KT=23,
   INDEX_SIGNAL_CONTACTOR_RPV1=21,
   INDEX_SIGNAL_CONTACTOR_RPN1=20,
   INDEX_SIGNAL_BLOCK_CIRCUT_STATUS=19,
   INDEX_SIGNAL_MU_TU_MODE=18};

	enum {
	INDEX_PARAM_MODE_VALUE=1,
	INDEX_PARAM_DKSE_POSITION=2,
	INDEX_PARAM_ESCALATOR_TYPE=3,
	INDEX_PARAM_BREAKING_PATH_VALUE=4,
	INDEX_PARAM_ESCALATOR_SPEED_VALUE=5,
	INDEX_PARAM_RUNNING_PATH_VALUE=6
};

	udku_data_block() :
	data_block( SIGNALS_COUNT,
					   MESSAGES_COUNT,
					   PARAMETERS_COUNT) {};

   virtual int get_data_block_type_description(){ return data_block::DATA_BLOCK_UDKU;};
   
   virtual ~udku_data_block(){};
};

#endif



