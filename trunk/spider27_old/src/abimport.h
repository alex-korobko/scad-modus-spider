/* Import (extern) header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

#include "abdefine.h"

extern ApEventLink_t AbInternalLinks[];

extern ApWindowLink_t main_wnd;
extern ApPictureLink_t interface;
extern ApDialogLink_t LogFilterDlg;
extern ApDialogLink_t EscConfig;
extern ApDialogLink_t SignalDlg;
extern ApDialogLink_t ArchiveDlg;
extern ApWindowLink_t ArchiveLog;
extern ApDialogLink_t PrevCmdPoolDlg;
extern ApWidget_t AbWidgets[ 135 ];

extern ApMenuLink_t EscalatorMenu;

#if defined(__cplusplus)
}
#endif


#ifdef __cplusplus
int Initialize( int argc, char **argv );
int Uninitialize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int CloseMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int NextAlert( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ActivateAlertBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_LogFilterOK( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realize_filter_dlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int combobox_close_StationList( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllRangeBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SelectEscalatorDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetUpDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetDownDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_ResetFilter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
int OnSaveDirections( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int PopupControlMenu( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ResizeScheme( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveUp( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveStop( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveDown( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OpenArchive( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realize_ArchiveLog( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealize_ArchiveLog( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int UnrealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetupPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetReverseDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnChancelDirections( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int FillEscConfList( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_command_pool_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
int activate_SendCommandButton( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_DeleteCommandBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized_CommandPoolRwLst( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_CommandPoolRwLst( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_CmdPoolBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_FilterBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_EscalatorsGroup( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_AllTimeRangeBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int activate_LoadArchiveBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int realized_main_log_wnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int unrealized_main_log_wnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
