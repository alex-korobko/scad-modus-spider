/* Define header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

/* Internal Module Links */
extern const int ABI_interface;
#define ABM_interface                        &AbInternalLinks[ABI_interface]
extern const int ABI_LogFilterDlg;
#define ABM_LogFilterDlg                     &AbInternalLinks[ABI_LogFilterDlg]
extern const int ABI_EscalatorMenu;
#define ABM_EscalatorMenu                    &AbInternalLinks[ABI_EscalatorMenu]
extern const int ABI_PrevCmdPoolDlg;
#define ABM_PrevCmdPoolDlg                   &AbInternalLinks[ABI_PrevCmdPoolDlg]
extern const int ABI_DeviceConfig;
#define ABM_DeviceConfig                     &AbInternalLinks[ABI_DeviceConfig]

/* 'main_wnd' Window link */
extern const int ABN_main_wnd;
#define ABW_main_wnd                         AbGetABW( ABN_main_wnd )
extern const int ABN_toolBar;
#define ABW_toolBar                          AbGetABW( ABN_toolBar )
extern const int ABN_esc_config_btn;
#define ABW_esc_config_btn                   AbGetABW( ABN_esc_config_btn )
extern const int ABN_ArchiveBtn;
#define ABW_ArchiveBtn                       AbGetABW( ABN_ArchiveBtn )
extern const int ABN_CmdPoolBtn;
#define ABW_CmdPoolBtn                       AbGetABW( ABN_CmdPoolBtn )
extern const int ABN_on_off_filter_main_log;
#define ABW_on_off_filter_main_log           AbGetABW( ABN_on_off_filter_main_log )
extern const int ABN_Scheme;
#define ABW_Scheme                           AbGetABW( ABN_Scheme )
extern const int ABN_AlertWnd;
#define ABW_AlertWnd                         AbGetABW( ABN_AlertWnd )
extern const int ABN_AlertMsg;
#define ABW_AlertMsg                         AbGetABW( ABN_AlertMsg )
extern const int ABN_AlertOK;
#define ABW_AlertOK                          AbGetABW( ABN_AlertOK )
extern const int ABN_AlertTime;
#define ABW_AlertTime                        AbGetABW( ABN_AlertTime )
extern const int ABN_AlertCounter;
#define ABW_AlertCounter                     AbGetABW( ABN_AlertCounter )
extern const int ABN_LogJournalPanel;
#define ABW_LogJournalPanel                  AbGetABW( ABN_LogJournalPanel )
extern const int ABN_main_log_wnd;
#define ABW_main_log_wnd                     AbGetABW( ABN_main_log_wnd )

/* 'interface' Picture link */
extern const int ABN_interface;
#define ABW_interface                        AbGetABW( ABN_interface )
extern const int ABN_shavr_wnd;
#define ABW_shavr_wnd                        AbGetABW( ABN_shavr_wnd )
extern const int ABN_input_1;
#define ABW_input_1                          AbGetABW( ABN_input_1 )
extern const int ABN_input_2;
#define ABW_input_2                          AbGetABW( ABN_input_2 )
extern const int ABN_escalator_wnd;
#define ABW_escalator_wnd                    AbGetABW( ABN_escalator_wnd )
extern const int ABN_Arrow;
#define ABW_Arrow                            AbGetABW( ABN_Arrow )
extern const int ABN_escalator_num;
#define ABW_escalator_num                    AbGetABW( ABN_escalator_num )
extern const int ABN_blocking;
#define ABW_blocking                         AbGetABW( ABN_blocking )
extern const int ABN_green_up;
#define ABW_green_up                         AbGetABW( ABN_green_up )
extern const int ABN_grey_stop;
#define ABW_grey_stop                        AbGetABW( ABN_grey_stop )
extern const int ABN_green_stop;
#define ABW_green_stop                       AbGetABW( ABN_green_stop )
extern const int ABN_red_up;
#define ABW_red_up                           AbGetABW( ABN_red_up )
extern const int ABN_red_stop;
#define ABW_red_stop                         AbGetABW( ABN_red_stop )
extern const int ABN_yellow_up;
#define ABW_yellow_up                        AbGetABW( ABN_yellow_up )
extern const int ABN_red_down;
#define ABW_red_down                         AbGetABW( ABN_red_down )
extern const int ABN_yellow_stop;
#define ABW_yellow_stop                      AbGetABW( ABN_yellow_stop )
extern const int ABN_yellow_down;
#define ABW_yellow_down                      AbGetABW( ABN_yellow_down )
extern const int ABN_infoMsg;
#define ABW_infoMsg                          AbGetABW( ABN_infoMsg )
extern const int ABN_errorMsg;
#define ABW_errorMsg                         AbGetABW( ABN_errorMsg )
extern const int ABN_warnMsg;
#define ABW_warnMsg                          AbGetABW( ABN_warnMsg )
extern const int ABN_block_green_led;
#define ABW_block_green_led                  AbGetABW( ABN_block_green_led )
extern const int ABN_block_blue_led;
#define ABW_block_blue_led                   AbGetABW( ABN_block_blue_led )
extern const int ABN_block_grey_led;
#define ABW_block_grey_led                   AbGetABW( ABN_block_grey_led )
extern const int ABN_block_red_led;
#define ABW_block_red_led                    AbGetABW( ABN_block_red_led )
extern const int ABN_block_yellow_led;
#define ABW_block_yellow_led                 AbGetABW( ABN_block_yellow_led )
extern const int ABN_grey_off;
#define ABW_grey_off                         AbGetABW( ABN_grey_off )
extern const int ABN_red_s_up;
#define ABW_red_s_up                         AbGetABW( ABN_red_s_up )
extern const int ABN_red_s_down;
#define ABW_red_s_down                       AbGetABW( ABN_red_s_down )
extern const int ABN_yellow_s_up;
#define ABW_yellow_s_up                      AbGetABW( ABN_yellow_s_up )
extern const int ABN_green_s_up;
#define ABW_green_s_up                       AbGetABW( ABN_green_s_up )
extern const int ABN_yellow_s_down;
#define ABW_yellow_s_down                    AbGetABW( ABN_yellow_s_down )
extern const int ABN_green_down;
#define ABW_green_down                       AbGetABW( ABN_green_down )
extern const int ABN_green_s_down;
#define ABW_green_s_down                     AbGetABW( ABN_green_s_down )
extern const int ABN_station_wnd;
#define ABW_station_wnd                      AbGetABW( ABN_station_wnd )
extern const int ABN_sensor_wnd;
#define ABW_sensor_wnd                       AbGetABW( ABN_sensor_wnd )
extern const int ABN_parameter_wnd;
#define ABW_parameter_wnd                    AbGetABW( ABN_parameter_wnd )
extern const int ABN_parameter_value;
#define ABW_parameter_value                  AbGetABW( ABN_parameter_value )
extern const int ABN_parameter_title;
#define ABW_parameter_title                  AbGetABW( ABN_parameter_title )
extern const int ABN_arrow_red_down;
#define ABW_arrow_red_down                   AbGetABW( ABN_arrow_red_down )
extern const int ABN_arrow_red_down_command;
#define ABW_arrow_red_down_command           AbGetABW( ABN_arrow_red_down_command )
extern const int ABN_arrow_red_down_small;
#define ABW_arrow_red_down_small             AbGetABW( ABN_arrow_red_down_small )
extern const int ABN_arrow_red_down_small_command;
#define ABW_arrow_red_down_small_command     AbGetABW( ABN_arrow_red_down_small_command )
extern const int ABN_arrow_green_down;
#define ABW_arrow_green_down                 AbGetABW( ABN_arrow_green_down )
extern const int ABN_arrow_green_down_command;
#define ABW_arrow_green_down_command         AbGetABW( ABN_arrow_green_down_command )
extern const int ABN_arrow_green_down_small;
#define ABW_arrow_green_down_small           AbGetABW( ABN_arrow_green_down_small )
extern const int ABN_arrow_green_down_small_command;
#define ABW_arrow_green_down_small_command   AbGetABW( ABN_arrow_green_down_small_command )
extern const int ABN_arrow_yellow_down;
#define ABW_arrow_yellow_down                AbGetABW( ABN_arrow_yellow_down )
extern const int ABN_arrow_yellow_down_command;
#define ABW_arrow_yellow_down_command        AbGetABW( ABN_arrow_yellow_down_command )
extern const int ABN_arrow_yellow_down_small;
#define ABW_arrow_yellow_down_small          AbGetABW( ABN_arrow_yellow_down_small )
extern const int ABN_arrow_yellow_down_small_command;
#define ABW_arrow_yellow_down_small_command  AbGetABW( ABN_arrow_yellow_down_small_command )
extern const int ABN_arrow_red_up;
#define ABW_arrow_red_up                     AbGetABW( ABN_arrow_red_up )
extern const int ABN_arrow_green_up;
#define ABW_arrow_green_up                   AbGetABW( ABN_arrow_green_up )
extern const int ABN_arrow_yellow_up;
#define ABW_arrow_yellow_up                  AbGetABW( ABN_arrow_yellow_up )
extern const int ABN_arrow_red_up_command;
#define ABW_arrow_red_up_command             AbGetABW( ABN_arrow_red_up_command )
extern const int ABN_arrow_red_up_small;
#define ABW_arrow_red_up_small               AbGetABW( ABN_arrow_red_up_small )
extern const int ABN_arrow_red_up_small_command;
#define ABW_arrow_red_up_small_command       AbGetABW( ABN_arrow_red_up_small_command )
extern const int ABN_arrow_green_up_command;
#define ABW_arrow_green_up_command           AbGetABW( ABN_arrow_green_up_command )
extern const int ABN_arrow_green_up_small;
#define ABW_arrow_green_up_small             AbGetABW( ABN_arrow_green_up_small )
extern const int ABN_arrow_green_up_small_command;
#define ABW_arrow_green_up_small_command     AbGetABW( ABN_arrow_green_up_small_command )
extern const int ABN_arrow_yellow_up_command;
#define ABW_arrow_yellow_up_command          AbGetABW( ABN_arrow_yellow_up_command )
extern const int ABN_arrow_yellow_up_small;
#define ABW_arrow_yellow_up_small            AbGetABW( ABN_arrow_yellow_up_small )
extern const int ABN_arrow_yellow_up_small_command;
#define ABW_arrow_yellow_up_small_command    AbGetABW( ABN_arrow_yellow_up_small_command )
extern const int ABN_shavr_disabled_1_disabled_2;
#define ABW_shavr_disabled_1_disabled_2      AbGetABW( ABN_shavr_disabled_1_disabled_2 )
extern const int ABN_shavr_enabled_1_disabled_2;
#define ABW_shavr_enabled_1_disabled_2       AbGetABW( ABN_shavr_enabled_1_disabled_2 )
extern const int ABN_shavr_disabled_1_enabled_2;
#define ABW_shavr_disabled_1_enabled_2       AbGetABW( ABN_shavr_disabled_1_enabled_2 )
extern const int ABN_shavr_enabled_1_enabled_2;
#define ABW_shavr_enabled_1_enabled_2        AbGetABW( ABN_shavr_enabled_1_enabled_2 )
extern const int ABN_offline;
#define ABW_offline                          AbGetABW( ABN_offline )
extern const int ABN_exception;
#define ABW_exception                        AbGetABW( ABN_exception )
extern const int ABN_block;
#define ABW_block                            AbGetABW( ABN_block )
extern const int ABN_shavr_widget;
#define ABW_shavr_widget                     AbGetABW( ABN_shavr_widget )
extern const int ABN_shavr_widget_panel_btn;
#define ABW_shavr_widget_panel_btn           AbGetABW( ABN_shavr_widget_panel_btn )
extern const int ABN_shavr_widget_command_lbl;
#define ABW_shavr_widget_command_lbl         AbGetABW( ABN_shavr_widget_command_lbl )
extern const int ABN_escalator_widget;
#define ABW_escalator_widget                 AbGetABW( ABN_escalator_widget )
extern const int ABN_escalator_widget_panel_btn;
#define ABW_escalator_widget_panel_btn       AbGetABW( ABN_escalator_widget_panel_btn )
extern const int ABN_escalator_number_lbl;
#define ABW_escalator_number_lbl             AbGetABW( ABN_escalator_number_lbl )
extern const int ABN_green_reverse;
#define ABW_green_reverse                    AbGetABW( ABN_green_reverse )
extern const int ABN_red_reverse;
#define ABW_red_reverse                      AbGetABW( ABN_red_reverse )
extern const int ABN_shavr_command;
#define ABW_shavr_command                    AbGetABW( ABN_shavr_command )
extern const int ABN_udku_widget;
#define ABW_udku_widget                      AbGetABW( ABN_udku_widget )
extern const int ABN_udku_widget_panel_btn;
#define ABW_udku_widget_panel_btn            AbGetABW( ABN_udku_widget_panel_btn )
extern const int ABN_udku_number_lbl;
#define ABW_udku_number_lbl                  AbGetABW( ABN_udku_number_lbl )

/* 'LogFilterDlg' Dialog link */
extern const int ABN_LogFilterDlg;
#define ABW_LogFilterDlg                     AbGetABW( ABN_LogFilterDlg )
extern const int ABN_AllTimeRangeBtn;
#define ABW_AllTimeRangeBtn                  AbGetABW( ABN_AllTimeRangeBtn )
extern const int ABN_StartTimePanel;
#define ABW_StartTimePanel                   AbGetABW( ABN_StartTimePanel )
extern const int ABN_StartHourNumeric;
#define ABW_StartHourNumeric                 AbGetABW( ABN_StartHourNumeric )
extern const int ABN_StartMinNumeric;
#define ABW_StartMinNumeric                  AbGetABW( ABN_StartMinNumeric )
extern const int ABN_CalendarStart;
#define ABW_CalendarStart                    AbGetABW( ABN_CalendarStart )
extern const int ABN_EndTimePanel;
#define ABW_EndTimePanel                     AbGetABW( ABN_EndTimePanel )
extern const int ABN_CalendarEnd;
#define ABW_CalendarEnd                      AbGetABW( ABN_CalendarEnd )
extern const int ABN_EndMinNumeric;
#define ABW_EndMinNumeric                    AbGetABW( ABN_EndMinNumeric )
extern const int ABN_EndHourNumeric;
#define ABW_EndHourNumeric                   AbGetABW( ABN_EndHourNumeric )
extern const int ABN_MessagesTypesList;
#define ABW_MessagesTypesList                AbGetABW( ABN_MessagesTypesList )
extern const int ABN_AllMessagesTypesBtn;
#define ABW_AllMessagesTypesBtn              AbGetABW( ABN_AllMessagesTypesBtn )
extern const int ABN_AllStationsBtn;
#define ABW_AllStationsBtn                   AbGetABW( ABN_AllStationsBtn )
extern const int ABN_StationsList;
#define ABW_StationsList                     AbGetABW( ABN_StationsList )
extern const int ABN_DevicesPane;
#define ABW_DevicesPane                      AbGetABW( ABN_DevicesPane )
extern const int ABN_AllDevicesBtn;
#define ABW_AllDevicesBtn                    AbGetABW( ABN_AllDevicesBtn )
extern const int ABN_DevicesList;
#define ABW_DevicesList                      AbGetABW( ABN_DevicesList )
extern const int ABN_LogFIlterCancel;
#define ABW_LogFIlterCancel                  AbGetABW( ABN_LogFIlterCancel )
extern const int ABN_LogFilterOK;
#define ABW_LogFilterOK                      AbGetABW( ABN_LogFilterOK )
extern const int ABN_FilteringBtn;
#define ABW_FilteringBtn                     AbGetABW( ABN_FilteringBtn )

/* 'DeviceConfig' Dialog link */
extern const int ABN_DeviceConfig;
#define ABW_DeviceConfig                     AbGetABW( ABN_DeviceConfig )
extern const int ABN_lbl_station;
#define ABW_lbl_station                      AbGetABW( ABN_lbl_station )
extern const int ABN_lbl_device;
#define ABW_lbl_device                       AbGetABW( ABN_lbl_device )
extern const int ABN_tglbtn_start_now;
#define ABW_tglbtn_start_now                 AbGetABW( ABN_tglbtn_start_now )
extern const int ABN_cb_start_day_mode;
#define ABW_cb_start_day_mode                AbGetABW( ABN_cb_start_day_mode )
extern const int ABN_cb_escalator_direction;
#define ABW_cb_escalator_direction           AbGetABW( ABN_cb_escalator_direction )
extern const int ABN_num_int_start_hour;
#define ABW_num_int_start_hour               AbGetABW( ABN_num_int_start_hour )
extern const int ABN_num_int_start_min;
#define ABW_num_int_start_min                AbGetABW( ABN_num_int_start_min )
extern const int ABN_cb_escalator_pref_direction;
#define ABW_cb_escalator_pref_direction      AbGetABW( ABN_cb_escalator_pref_direction )
extern const int ABN_esc_conf_cancel;
#define ABW_esc_conf_cancel                  AbGetABW( ABN_esc_conf_cancel )
extern const int ABN_esc_conf_save;
#define ABW_esc_conf_save                    AbGetABW( ABN_esc_conf_save )
extern const int ABN_cb_esc_config_list_mode;
#define ABW_cb_esc_config_list_mode          AbGetABW( ABN_cb_esc_config_list_mode )
extern const int ABN_cb_lines;
#define ABW_cb_lines                         AbGetABW( ABN_cb_lines )
extern const int ABN_rwlst_config_devices;
#define ABW_rwlst_config_devices             AbGetABW( ABN_rwlst_config_devices )

/* 'SignalDlg' Dialog link */
extern const int ABN_SignalDlg;
#define ABW_SignalDlg                        AbGetABW( ABN_SignalDlg )
extern const int ABN_SignalPanel;
#define ABW_SignalPanel                      AbGetABW( ABN_SignalPanel )
extern const int ABN_mode_header;
#define ABW_mode_header                      AbGetABW( ABN_mode_header )
extern const int ABN_ModeIndicator;
#define ABW_ModeIndicator                    AbGetABW( ABN_ModeIndicator )
extern const int ABN_ReadyIndicator;
#define ABW_ReadyIndicator                   AbGetABW( ABN_ReadyIndicator )
extern const int ABN_ready_header;
#define ABW_ready_header                     AbGetABW( ABN_ready_header )
extern const int ABN_RunningIndicator;
#define ABW_RunningIndicator                 AbGetABW( ABN_RunningIndicator )
extern const int ABN_running_header;
#define ABW_running_header                   AbGetABW( ABN_running_header )
extern const int ABN_BreakingPathIndicator;
#define ABW_BreakingPathIndicator            AbGetABW( ABN_BreakingPathIndicator )
extern const int ABN_breaking_path_header;
#define ABW_breaking_path_header             AbGetABW( ABN_breaking_path_header )
extern const int ABN_PanelTitle;
#define ABW_PanelTitle                       AbGetABW( ABN_PanelTitle )

/* 'ArchiveDlg' Dialog link */
extern const int ABN_ArchiveDlg;
#define ABW_ArchiveDlg                       AbGetABW( ABN_ArchiveDlg )
extern const int ABN_ArchiveFiles;
#define ABW_ArchiveFiles                     AbGetABW( ABN_ArchiveFiles )
extern const int ABN_LoadArchiveBtn;
#define ABW_LoadArchiveBtn                   AbGetABW( ABN_LoadArchiveBtn )
extern const int ABN_ArchiveCancelBtn;
#define ABW_ArchiveCancelBtn                 AbGetABW( ABN_ArchiveCancelBtn )

/* 'ArchiveLog' Window link */
extern const int ABN_ArchiveLog;
#define ABW_ArchiveLog                       AbGetABW( ABN_ArchiveLog )
extern const int ABN_ArchiveLogWnd;
#define ABW_ArchiveLogWnd                    AbGetABW( ABN_ArchiveLogWnd )
extern const int ABN_ArchiveLogCancelBtn;
#define ABW_ArchiveLogCancelBtn              AbGetABW( ABN_ArchiveLogCancelBtn )
extern const int ABN_ArchiveLogOpenBtn;
#define ABW_ArchiveLogOpenBtn                AbGetABW( ABN_ArchiveLogOpenBtn )
extern const int ABN_on_off_filter_archive_log;
#define ABW_on_off_filter_archive_log        AbGetABW( ABN_on_off_filter_archive_log )

/* 'PrevCmdPoolDlg' Dialog link */
extern const int ABN_PrevCmdPullDlg;
#define ABW_PrevCmdPullDlg                   AbGetABW( ABN_PrevCmdPullDlg )
extern const int ABN_CmdPoolDlgBtnClose;
#define ABW_CmdPoolDlgBtnClose               AbGetABW( ABN_CmdPoolDlgBtnClose )
extern const int ABN_SendCommandButton;
#define ABW_SendCommandButton                AbGetABW( ABN_SendCommandButton )
extern const int ABN_DeleteCommandBtn;
#define ABW_DeleteCommandBtn                 AbGetABW( ABN_DeleteCommandBtn )
extern const int ABN_CommandPoolRwLst;
#define ABW_CommandPoolRwLst                 AbGetABW( ABN_CommandPoolRwLst )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

/* 'EscalatorMenu' Menu link */
extern const int ABN_EscalatorMenu;
extern const int ABN_MoveUp;
extern const int ABN_MoveStop;
extern const int ABN_MoveDown;

#define AB_OPTIONS "s:"

#if defined(__cplusplus)
}
#endif

