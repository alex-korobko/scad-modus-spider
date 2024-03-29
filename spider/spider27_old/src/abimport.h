/* Import (extern) header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern ApEventLink_t AbInternalLinks[];

extern ApWindowLink_t main_wnd;
extern ApPictureLink_t interface;
extern ApDialogLink_t LogFilterDlg;
extern ApDialogLink_t DeviceConfig;
extern ApDialogLink_t SignalDlg;
extern ApDialogLink_t ArchiveDlg;
extern ApWindowLink_t ArchiveLog;
extern ApDialogLink_t PrevCmdPoolDlg;
extern ApWidget_t AbWidgets[ 155 ];

extern ApMenuLink_t EscalatorMenu;

#if defined(__cplusplus)
}
#endif


#ifdef __cplusplus
int Initialize( int argc, char **argv );
int Uninitialize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int CloseMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ActivateAlertBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int link_setup_LogFilterDlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
int PopupControlMenu( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ResizeScheme( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveUp( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveStop( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveDown( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OpenArchive( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int UnrealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetupPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int link_setup_PrevCmdPullDlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_command_pool_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
int activate_SendCommandButton( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_DeleteCommandBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_CmdPoolBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllTimeRangeBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_LoadArchiveBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_LogFilterDlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllStationsBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllDevicesBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllMessagesTypesBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int itemselection_StationList( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int itemselection_DevicesList( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_LogFilterOK( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_on_off_filter_main_log( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized_ArchiveLog( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_ArchiveLog( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_on_off_filter_archive_log( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int link_setup_EscConfig( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_esc_config_btn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int item_selection_cb_lines( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int item_selection_cb_esc_config_list_mode( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int item_selection_cb_start_day_mode( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int item_selection_cb_escalator_direction( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int item_selection_cb_escalator_pref_direction( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_EscConfig( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_escalator_start_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
void selection_escalator_start_item(
        PtWidget_t *widget, PtGenListItem_t *item, int pos, int column,
        int nitems, int subtype, PhEvent_t *ev
        );
int activate_tglbtn_start_now( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int changed_num_int_start_time( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_esc_conf_save( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_PrevCmdPullDlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
