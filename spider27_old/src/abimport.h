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
extern ApWidget_t AbWidgets[ 118 ];

extern ApMenuLink_t EscalatorMenu;

#if defined(__cplusplus)
}
#endif


#ifdef __cplusplus
int Initialize( int argc, char **argv );
int Uninitialize( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int select_escalator( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int CloseMainWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int NextAlert( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ActivateAlertBtn( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetLogFilter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeFilterDlg( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int EnableAllTimeRange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int DisableAllTimeRange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SelectFilterStation( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ActivateOneEscalator( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ActivateRangeEscalator( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SelectAllRange( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SelectEscalatorDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetUpDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetDownDirection( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ResetFilter( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
void draw_log_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        );
int realize_log_wnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnSaveDirections( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int PopupControlMenu( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ResizeScheme( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveUp( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveStop( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int MoveDown( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OpenArchive( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RealizeArchiveWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int UnrealizeArchiveWnd( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int LoadArchive( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int UnrealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int RelizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetupPanel( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int DiscardBlocking( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int ResizeEscConfig( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int SetReverseEscalator( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int OnChancelDirections( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int FillEscConfList( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#endif
