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
        0 LT-1 0,75
        1 LT-2 0,90
        2 LT-3 0,90
        3 LT-4 0,88
        4 LT-5 0,90
        5 ET-2 0,94
        6 ET-3 0,94
        7 ET-5 0,73
        8 EM-4 0,94
        9 EM-5.5S 0,94
        10 EM-5.5T 0,75
        11 ET-5/h 0,73

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

	modbus register 30006
     high byte
          fourth byte of escalator running path (porbeg)
     low byte
          third byte of escalator running path (porbeg)

	modbus register 30007
     high byte
          second byte of escalator running path (porbeg)
     low byte
          first byte of escalator running path (porbeg)

	modbus register 30008 (UDKU inputs  6 and 7 bits used for block circut (other block-circut contacts on 30009 register))
     high byte
        bit number (1 active state)
        7 R3+RKA+RAT+UKPE
        6 BT1+BT2
        5 DU0 escalator stopped from DU - pulse
        4 KNS1 button STOP (1 mean button is pressed)
        3 Not used
        2 Mashzal door
        1 RKN2 circut of ~110V power supply control (1 mean ~110V power supply NOT PRESENT)
        0 RKN1 circut of ~380V power supply control (1 mean ~380V power supply NOT PRESENT)
     low byte
        bit number (1 active state)
        7 RKP rele of  start control (1 mean escalator rumming on nominal speed, 0 mean escalator is stopped or in starting mode)
        6 RB2 circut of power supply (1 mean is power supply NOT norma)
        5 KT breaks kontaktor (1 mean breaks is ON)
        4 RG rele ready to start from TU (1 mean not ready)
        3 RPV1 kontactor  UP (1 mean escalator running UP)
        2 RPN1 kontactor DOWN (1 mean escalator running DOWN)
        1 Block circut is norma (1 mean block circut is NOT norma)
        0 Mode TU/MU (1 mean TU mode)

	modbus register 30009 (UDKU inputs - block circut contacts)
     high byte
        bit number (1 active state)
        7 RTL+RTP
        6 VNL+VNP
        5 KZP3+KS4
        4 KANL+KANP+RDO
        3 VSN
        2 VVNL+VVNP
        1 VPNL+VPNP
        0 VPVL+VPVP

     low byte
        7 RUL+RUP
        6 VPP+VPL
        5 VOL+VOP
        4 BSV
        3 VVVL+VVVP
        2 KS2+KZP2+KS3
        1 KAVL+KAVP
        0 VG+VA+RUA

	modbus register 30009
      upper message id in messages pull

	modbus register 30010
      messages count in messages pull

	modbus register form 30011 to 30011+COUNT_OF_MESSAGES
     messages pull
*/


typedef struct udku_data_block : public data_block{
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



