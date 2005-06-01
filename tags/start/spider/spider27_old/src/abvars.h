/* Variables header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

/* Internal Module Links */
const int ABI_interface = 0;
const int ABI_LogFilterDlg = 1;
const int ABI_EscalatorMenu = 2;
const int ABI_PrevCmdPoolDlg = 3;
const int ABI_DeviceConfig = 4;

/* 'main_wnd' Window link */
const int ABN_main_wnd = 0;
const int ABN_toolBar = 1;
const int ABN_esc_config_btn = 2;
const int ABN_ArchiveBtn = 3;
const int ABN_CmdPoolBtn = 4;
const int ABN_on_off_filter_main_log = 5;
const int ABN_Scheme = 6;
const int ABN_AlertWnd = 7;
const int ABN_AlertMsg = 8;
const int ABN_AlertOK = 9;
const int ABN_AlertTime = 10;
const int ABN_AlertCounter = 11;
const int ABN_LogJournalPanel = 12;
const int ABN_main_log_wnd = 13;

/* 'interface' Picture link */
const int ABN_interface = 14;
const int ABN_shavr_wnd = 15;
const int ABN_input_1 = 16;
const int ABN_input_2 = 17;
const int ABN_escalator_wnd = 18;
const int ABN_Arrow = 19;
const int ABN_escalator_num = 20;
const int ABN_blocking = 21;
const int ABN_green_up = 22;
const int ABN_grey_stop = 23;
const int ABN_green_stop = 24;
const int ABN_red_up = 25;
const int ABN_red_stop = 26;
const int ABN_yellow_up = 27;
const int ABN_red_down = 28;
const int ABN_yellow_stop = 29;
const int ABN_yellow_down = 30;
const int ABN_infoMsg = 31;
const int ABN_errorMsg = 32;
const int ABN_warnMsg = 33;
const int ABN_block_green_led = 34;
const int ABN_block_blue_led = 35;
const int ABN_block_grey_led = 36;
const int ABN_block_red_led = 37;
const int ABN_block_yellow_led = 38;
const int ABN_grey_off = 39;
const int ABN_red_s_up = 40;
const int ABN_red_s_down = 41;
const int ABN_yellow_s_up = 42;
const int ABN_green_s_up = 43;
const int ABN_yellow_s_down = 44;
const int ABN_green_down = 45;
const int ABN_green_s_down = 46;
const int ABN_station_wnd = 47;
const int ABN_sensor_wnd = 48;
const int ABN_parameter_wnd = 49;
const int ABN_parameter_value = 50;
const int ABN_parameter_title = 51;
const int ABN_arrow_red_down = 52;
const int ABN_arrow_red_down_command = 53;
const int ABN_arrow_red_down_small = 54;
const int ABN_arrow_red_down_small_command = 55;
const int ABN_arrow_green_down = 56;
const int ABN_arrow_green_down_command = 57;
const int ABN_arrow_green_down_small = 58;
const int ABN_arrow_green_down_small_command = 59;
const int ABN_arrow_yellow_down = 60;
const int ABN_arrow_yellow_down_command = 61;
const int ABN_arrow_yellow_down_small = 62;
const int ABN_arrow_yellow_down_small_command = 63;
const int ABN_arrow_red_up = 64;
const int ABN_arrow_green_up = 65;
const int ABN_arrow_yellow_up = 66;
const int ABN_arrow_red_up_command = 67;
const int ABN_arrow_red_up_small = 68;
const int ABN_arrow_red_up_small_command = 69;
const int ABN_arrow_green_up_command = 70;
const int ABN_arrow_green_up_small = 71;
const int ABN_arrow_green_up_small_command = 72;
const int ABN_arrow_yellow_up_command = 73;
const int ABN_arrow_yellow_up_small = 74;
const int ABN_arrow_yellow_up_small_command = 75;
const int ABN_shavr_disabled_1_disabled_2 = 76;
const int ABN_shavr_enabled_1_disabled_2 = 77;
const int ABN_shavr_disabled_1_enabled_2 = 78;
const int ABN_shavr_enabled_1_enabled_2 = 79;
const int ABN_offline = 80;
const int ABN_exception = 81;
const int ABN_block = 82;
const int ABN_shavr_widget = 83;
const int ABN_shavr_widget_panel_btn = 84;
const int ABN_shavr_widget_command_lbl = 85;
const int ABN_escalator_widget = 86;
const int ABN_escalator_widget_panel_btn = 87;
const int ABN_escalator_number_lbl = 88;
const int ABN_green_reverse = 89;
const int ABN_red_reverse = 90;
const int ABN_shavr_command = 91;
const int ABN_udku_widget = 92;
const int ABN_udku_widget_panel_btn = 93;
const int ABN_udku_number_lbl = 94;

/* 'LogFilterDlg' Dialog link */
const int ABN_LogFilterDlg = 95;
const int ABN_AllTimeRangeBtn = 96;
const int ABN_StartTimePanel = 97;
const int ABN_StartHourNumeric = 98;
const int ABN_StartMinNumeric = 99;
const int ABN_CalendarStart = 100;
const int ABN_EndTimePanel = 101;
const int ABN_CalendarEnd = 102;
const int ABN_EndMinNumeric = 103;
const int ABN_EndHourNumeric = 104;
const int ABN_MessagesTypesList = 105;
const int ABN_AllMessagesTypesBtn = 106;
const int ABN_AllStationsBtn = 107;
const int ABN_StationsList = 108;
const int ABN_DevicesPane = 109;
const int ABN_AllDevicesBtn = 110;
const int ABN_DevicesList = 111;
const int ABN_LogFIlterCancel = 112;
const int ABN_LogFilterOK = 113;
const int ABN_FilteringBtn = 114;

/* 'DeviceConfig' Dialog link */
const int ABN_DeviceConfig = 115;
const int ABN_lbl_station = 116;
const int ABN_lbl_device = 117;
const int ABN_tglbtn_start_now = 118;
const int ABN_cb_start_day_mode = 119;
const int ABN_cb_escalator_direction = 120;
const int ABN_num_int_start_hour = 121;
const int ABN_num_int_start_min = 122;
const int ABN_cb_escalator_pref_direction = 123;
const int ABN_esc_conf_cancel = 124;
const int ABN_esc_conf_save = 125;
const int ABN_cb_esc_config_list_mode = 126;
const int ABN_cb_lines = 127;
const int ABN_rwlst_config_devices = 128;

/* 'SignalDlg' Dialog link */
const int ABN_SignalDlg = 129;
const int ABN_SignalPanel = 130;
const int ABN_mode_header = 131;
const int ABN_ModeIndicator = 132;
const int ABN_ReadyIndicator = 133;
const int ABN_ready_header = 134;
const int ABN_RunningIndicator = 135;
const int ABN_running_header = 136;
const int ABN_BreakingPathIndicator = 137;
const int ABN_breaking_path_header = 138;
const int ABN_PanelTitle = 139;

/* 'ArchiveDlg' Dialog link */
const int ABN_ArchiveDlg = 140;
const int ABN_ArchiveFiles = 141;
const int ABN_LoadArchiveBtn = 142;
const int ABN_ArchiveCancelBtn = 143;

/* 'ArchiveLog' Window link */
const int ABN_ArchiveLog = 144;
const int ABN_ArchiveLogWnd = 145;
const int ABN_ArchiveLogCancelBtn = 146;
const int ABN_ArchiveLogOpenBtn = 147;
const int ABN_on_off_filter_archive_log = 148;

/* 'PrevCmdPoolDlg' Dialog link */
const int ABN_PrevCmdPullDlg = 149;
const int ABN_CmdPoolDlgBtnClose = 150;
const int ABN_SendCommandButton = 151;
const int ABN_DeleteCommandBtn = 152;
const int ABN_CommandPoolRwLst = 153;

/* 'EscalatorMenu' Menu link */
const int ABN_EscalatorMenu = 154;
const int ABN_MoveUp = 155;
const int ABN_MoveStop = 156;
const int ABN_MoveDown = 157;


#if defined(__cplusplus)
}
#endif

