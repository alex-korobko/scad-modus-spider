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

/* 'main_wnd' Window link */
extern const int ABN_main_wnd;
#define ABW_main_wnd                         AbGetABW( ABN_main_wnd )
extern const int ABN_os_container;
#define ABW_os_container                     AbGetABW( ABN_os_container )
extern const int ABN_log_status;
#define ABW_log_status                       AbGetABW( ABN_log_status )
extern const int ABN_log_wnd;
#define ABW_log_wnd                          AbGetABW( ABN_log_wnd )
extern const int ABN_toolBar;
#define ABW_toolBar                          AbGetABW( ABN_toolBar )
extern const int ABN_EscConfigBtn;
#define ABW_EscConfigBtn                     AbGetABW( ABN_EscConfigBtn )
extern const int ABN_FilterBtn;
#define ABW_FilterBtn                        AbGetABW( ABN_FilterBtn )
extern const int ABN_ResetFilter;
#define ABW_ResetFilter                      AbGetABW( ABN_ResetFilter )
extern const int ABN_ArchiveBtn;
#define ABW_ArchiveBtn                       AbGetABW( ABN_ArchiveBtn )
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
extern const int ABN_Scheme;
#define ABW_Scheme                           AbGetABW( ABN_Scheme )

/* 'interface' Picture link */
extern const int ABN_interface;
#define ABW_interface                        AbGetABW( ABN_interface )
extern const int ABN_escalator_wnd;
#define ABW_escalator_wnd                    AbGetABW( ABN_escalator_wnd )
extern const int ABN_Arrow;
#define ABW_Arrow                            AbGetABW( ABN_Arrow )
extern const int ABN_escalator_num;
#define ABW_escalator_num                    AbGetABW( ABN_escalator_num )
extern const int ABN_blocking;
#define ABW_blocking                         AbGetABW( ABN_blocking )
extern const int ABN_left_scroll_disable;
#define ABW_left_scroll_disable              AbGetABW( ABN_left_scroll_disable )
extern const int ABN_right_scroll_disable;
#define ABW_right_scroll_disable             AbGetABW( ABN_right_scroll_disable )
extern const int ABN_station_wnd;
#define ABW_station_wnd                      AbGetABW( ABN_station_wnd )
extern const int ABN_green_up;
#define ABW_green_up                         AbGetABW( ABN_green_up )
extern const int ABN_grey_stop;
#define ABW_grey_stop                        AbGetABW( ABN_grey_stop )
extern const int ABN_grey_down;
#define ABW_grey_down                        AbGetABW( ABN_grey_down )
extern const int ABN_led_b;
#define ABW_led_b                            AbGetABW( ABN_led_b )
extern const int ABN_blue_down;
#define ABW_blue_down                        AbGetABW( ABN_blue_down )
extern const int ABN_green_stop;
#define ABW_green_stop                       AbGetABW( ABN_green_stop )
extern const int ABN_blue_up;
#define ABW_blue_up                          AbGetABW( ABN_blue_up )
extern const int ABN_green_led;
#define ABW_green_led                        AbGetABW( ABN_green_led )
extern const int ABN_led_tu;
#define ABW_led_tu                           AbGetABW( ABN_led_tu )
extern const int ABN_red_up;
#define ABW_red_up                           AbGetABW( ABN_red_up )
extern const int ABN_red_stop;
#define ABW_red_stop                         AbGetABW( ABN_red_stop )
extern const int ABN_red_down;
#define ABW_red_down                         AbGetABW( ABN_red_down )
extern const int ABN_yellow_up;
#define ABW_yellow_up                        AbGetABW( ABN_yellow_up )
extern const int ABN_yellow_stop;
#define ABW_yellow_stop                      AbGetABW( ABN_yellow_stop )
extern const int ABN_yellow_down;
#define ABW_yellow_down                      AbGetABW( ABN_yellow_down )
extern const int ABN_red_led;
#define ABW_red_led                          AbGetABW( ABN_red_led )
extern const int ABN_yellow_led;
#define ABW_yellow_led                       AbGetABW( ABN_yellow_led )
extern const int ABN_blue_led;
#define ABW_blue_led                         AbGetABW( ABN_blue_led )
extern const int ABN_mask_up;
#define ABW_mask_up                          AbGetABW( ABN_mask_up )
extern const int ABN_mask_stop;
#define ABW_mask_stop                        AbGetABW( ABN_mask_stop )
extern const int ABN_mask_down;
#define ABW_mask_down                        AbGetABW( ABN_mask_down )
extern const int ABN_cursor;
#define ABW_cursor                           AbGetABW( ABN_cursor )
extern const int ABN_log_alert_image;
#define ABW_log_alert_image                  AbGetABW( ABN_log_alert_image )
extern const int ABN_log_ok_image;
#define ABW_log_ok_image                     AbGetABW( ABN_log_ok_image )
extern const int ABN_log_net_image;
#define ABW_log_net_image                    AbGetABW( ABN_log_net_image )
extern const int ABN_infoMsg;
#define ABW_infoMsg                          AbGetABW( ABN_infoMsg )
extern const int ABN_stopMsg;
#define ABW_stopMsg                          AbGetABW( ABN_stopMsg )
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
extern const int ABN_blue_cont_up;
#define ABW_blue_cont_up                     AbGetABW( ABN_blue_cont_up )
extern const int ABN_blue_cont_down;
#define ABW_blue_cont_down                   AbGetABW( ABN_blue_cont_down )
extern const int ABN_blue_red_up;
#define ABW_blue_red_up                      AbGetABW( ABN_blue_red_up )
extern const int ABN_blue_red_down;
#define ABW_blue_red_down                    AbGetABW( ABN_blue_red_down )
extern const int ABN_blue_yellow_up;
#define ABW_blue_yellow_up                   AbGetABW( ABN_blue_yellow_up )
extern const int ABN_blue_yellow_down;
#define ABW_blue_yellow_down                 AbGetABW( ABN_blue_yellow_down )
extern const int ABN_red_s_down;
#define ABW_red_s_down                       AbGetABW( ABN_red_s_down )
extern const int ABN_red_s_up;
#define ABW_red_s_up                         AbGetABW( ABN_red_s_up )
extern const int ABN_yellow_s_up;
#define ABW_yellow_s_up                      AbGetABW( ABN_yellow_s_up )
extern const int ABN_yellow_s_down;
#define ABW_yellow_s_down                    AbGetABW( ABN_yellow_s_down )
extern const int ABN_green_down;
#define ABW_green_down                       AbGetABW( ABN_green_down )
extern const int ABN_green_s_up;
#define ABW_green_s_up                       AbGetABW( ABN_green_s_up )
extern const int ABN_green_s_down;
#define ABW_green_s_down                     AbGetABW( ABN_green_s_down )
extern const int ABN_LedBalloon;
#define ABW_LedBalloon                       AbGetABW( ABN_LedBalloon )

/* 'LogFilterDlg' Dialog link */
extern const int ABN_LogFilterDlg;
#define ABW_LogFilterDlg                     AbGetABW( ABN_LogFilterDlg )
extern const int ABN_StartMinNumeric;
#define ABW_StartMinNumeric                  AbGetABW( ABN_StartMinNumeric )
extern const int ABN_StartHourNumeric;
#define ABW_StartHourNumeric                 AbGetABW( ABN_StartHourNumeric )
extern const int ABN_CalendarStart;
#define ABW_CalendarStart                    AbGetABW( ABN_CalendarStart )
extern const int ABN_AllTimeRangeBtn1;
#define ABW_AllTimeRangeBtn1                 AbGetABW( ABN_AllTimeRangeBtn1 )
extern const int ABN_EndMinNumeric;
#define ABW_EndMinNumeric                    AbGetABW( ABN_EndMinNumeric )
extern const int ABN_EndHourNumeric;
#define ABW_EndHourNumeric                   AbGetABW( ABN_EndHourNumeric )
extern const int ABN_CalendarEnd;
#define ABW_CalendarEnd                      AbGetABW( ABN_CalendarEnd )
extern const int ABN_AllTimeRangeBtn2;
#define ABW_AllTimeRangeBtn2                 AbGetABW( ABN_AllTimeRangeBtn2 )
extern const int ABN_StationList;
#define ABW_StationList                      AbGetABW( ABN_StationList )
extern const int ABN_OneEscalatorBtn;
#define ABW_OneEscalatorBtn                  AbGetABW( ABN_OneEscalatorBtn )
extern const int ABN_RangeEscalatorBtn;
#define ABW_RangeEscalatorBtn                AbGetABW( ABN_RangeEscalatorBtn )
extern const int ABN_OneEscalatorLabel;
#define ABW_OneEscalatorLabel                AbGetABW( ABN_OneEscalatorLabel )
extern const int ABN_OneEscalatorNumeric;
#define ABW_OneEscalatorNumeric              AbGetABW( ABN_OneEscalatorNumeric )
extern const int ABN_RangeStartLabel;
#define ABW_RangeStartLabel                  AbGetABW( ABN_RangeStartLabel )
extern const int ABN_RangeStartNumeric;
#define ABW_RangeStartNumeric                AbGetABW( ABN_RangeStartNumeric )
extern const int ABN_RangeEndLabel;
#define ABW_RangeEndLabel                    AbGetABW( ABN_RangeEndLabel )
extern const int ABN_RangeEndNumeric;
#define ABW_RangeEndNumeric                  AbGetABW( ABN_RangeEndNumeric )
extern const int ABN_AllRangeBtn;
#define ABW_AllRangeBtn                      AbGetABW( ABN_AllRangeBtn )
extern const int ABN_SysMsgBtn;
#define ABW_SysMsgBtn                        AbGetABW( ABN_SysMsgBtn )
extern const int ABN_AlertMsgBtn;
#define ABW_AlertMsgBtn                      AbGetABW( ABN_AlertMsgBtn )
extern const int ABN_BlockMsgBtn;
#define ABW_BlockMsgBtn                      AbGetABW( ABN_BlockMsgBtn )
extern const int ABN_LocalMsgBtn;
#define ABW_LocalMsgBtn                      AbGetABW( ABN_LocalMsgBtn )
extern const int ABN_LogFIlterCancel;
#define ABW_LogFIlterCancel                  AbGetABW( ABN_LogFIlterCancel )
extern const int ABN_LogFilterOK;
#define ABW_LogFilterOK                      AbGetABW( ABN_LogFilterOK )

/* 'EscConfig' Dialog link */
extern const int ABN_EscConfig;
#define ABW_EscConfig                        AbGetABW( ABN_EscConfig )
extern const int ABN_EscDirectionList;
#define ABW_EscDirectionList                 AbGetABW( ABN_EscDirectionList )
extern const int ABN_SetEscReverseBtn;
#define ABW_SetEscReverseBtn                 AbGetABW( ABN_SetEscReverseBtn )
extern const int ABN_SetEscUpBtn;
#define ABW_SetEscUpBtn                      AbGetABW( ABN_SetEscUpBtn )
extern const int ABN_SetEscDownBtn;
#define ABW_SetEscDownBtn                    AbGetABW( ABN_SetEscDownBtn )
extern const int ABN_EscConfCancel;
#define ABW_EscConfCancel                    AbGetABW( ABN_EscConfCancel )
extern const int ABN_EscConfDone;
#define ABW_EscConfDone                      AbGetABW( ABN_EscConfDone )

/* 'SignalDlg' Dialog link */
extern const int ABN_SignalDlg;
#define ABW_SignalDlg                        AbGetABW( ABN_SignalDlg )
extern const int ABN_SignalPanel;
#define ABW_SignalPanel                      AbGetABW( ABN_SignalPanel )
extern const int ABN_mode_header;
#define ABW_mode_header                      AbGetABW( ABN_mode_header )
extern const int ABN_ModeIndicator;
#define ABW_ModeIndicator                    AbGetABW( ABN_ModeIndicator )
extern const int ABN_ready_header;
#define ABW_ready_header                     AbGetABW( ABN_ready_header )
extern const int ABN_ReadyIndicator;
#define ABW_ReadyIndicator                   AbGetABW( ABN_ReadyIndicator )
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
extern const int ABN_ArchiveLogOpenBtn;
#define ABW_ArchiveLogOpenBtn                AbGetABW( ABN_ArchiveLogOpenBtn )
extern const int ABN_ArchiveLogCancelBtn;
#define ABW_ArchiveLogCancelBtn              AbGetABW( ABN_ArchiveLogCancelBtn )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

/* 'EscalatorMenu' Menu link */
extern const int ABN_EscalatorMenu;
extern const int ABN_MoveUp;
extern const int ABN_MoveStop;
extern const int ABN_MoveDown;

#define AB_OPTIONS "s:x:y:h:w:S:"

#if defined(__cplusplus)
}
#endif

