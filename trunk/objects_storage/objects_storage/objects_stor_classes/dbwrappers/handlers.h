#ifndef HANDLERS_H__
#define HANDLERS_H__

#ifdef __cplusplus
extern "C"
{
#endif

	int unregister_events(mco_db_h db);
	int register_events(mco_db_h db);
	MCO_RET update_channel_b_handler(mco_trans_h t, CHANNEL_B  * obj,  MCO_EVENT_TYPE et,/* INOUT */void *param);
	MCO_RET new_channel_b_in_trunk(mco_trans_h t, CH_B_TRUNK_GR*  obj, MCO_EVENT_TYPE et, /* INOUT */ void *param);

#ifdef __cplusplus
}
#endif
#endif
