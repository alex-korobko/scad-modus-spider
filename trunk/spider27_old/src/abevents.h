/* Event header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &main_wnd, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

ApEventLink_t AbInternalLinks[] = {
	{ 11, 0, 0L, 0L, 0L, &interface, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 4, 0, 0L, 0L, 0L, &LogFilterDlg, NULL, NULL, 0, NULL, 0, 9, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &EscalatorMenu, NULL, NULL, 0, NULL, 0, 3, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_main_wnd[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "main_wnd", 1009, Uninitialize, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "main_wnd", 1012, RealizeMainWnd, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "main_wnd", 18024, CloseMainWnd, 0, 0, 0, 0, },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "log_wnd", 68000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) draw_log_item, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "log_wnd", 1012, realize_log_wnd, 0, 0, 0, 0, },
	{ 4, 0, 0L, 0L, 0L, &EscConfig, NULL, "EscConfigBtn", 2009, NULL, 0, 0, 0, 0, },
	{ 4, 0, 0L, 0L, 0L, &LogFilterDlg, NULL, "FilterBtn", 2009, NULL, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ResetFilter", 2009, ResetFilter, 0, 0, 0, 0, },
	{ 3, 0, 0L, 0L, 0L, &ArchiveLog, NULL, "ArchiveBtn", 2009, OpenArchive, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AlertOK", 2007, NextAlert, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AlertOK", 2009, ActivateAlertBtn, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Scheme", 10003, ResizeScheme, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_interface[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "escalator_wnd", 2009, select_escalator, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "escalator_wnd", 2016, PopupControlMenu, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "Arrow", 2009, SetupPanel, 0, 0, 0, 0, },
	{ 4, 0, 0L, 0L, 0L, &SignalDlg, NULL, "Arrow", 2009, NULL, 0, 9, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "blocking", 2009, DiscardBlocking, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_LogFilterDlg[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "LogFilterDlg", 1012, RealizeFilterDlg, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AllTimeRangeBtn1", 2007, EnableAllTimeRange, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AllTimeRangeBtn1", 2008, DisableAllTimeRange, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AllTimeRangeBtn2", 2007, EnableAllTimeRange, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AllTimeRangeBtn2", 2008, DisableAllTimeRange, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "StationList", 31020, SelectFilterStation, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "OneEscalatorBtn", 2009, ActivateOneEscalator, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "RangeEscalatorBtn", 2009, ActivateRangeEscalator, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "AllRangeBtn", 2009, SelectAllRange, 0, 0, 0, 0, },
	{ 10, 0, 0L, 0L, 0L, NULL, NULL, "LogFIlterCancel", 2007, NULL, 0, 0, 0, 0, },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "LogFilterOK", 2007, SetLogFilter, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_EscConfig[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "EscConfig", 1012, FillEscConfList, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "EscDirectionList", 23010, SelectEscalatorDirection, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SetEscReverseBtn", 2009, SetReverseEscalator, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SetEscDownBtn", 2009, SetDownDirection, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SetEscUpBtn", 2009, SetUpDirection, 0, 0, 0, 0, },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "EscConfCancel", 2009, OnChancelDirections, 0, 0, 0, 0, },
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "EscConfDone", 2009, OnSaveDirections, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_SignalDlg[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SignalDlg", 1013, UnrealizeEscalatorPanel, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "SignalDlg", 1012, RelizeEscalatorPanel, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_ArchiveDlg[] = {
	{ 9, 0, 0L, 0L, 0L, NULL, NULL, "LoadArchiveBtn", 2009, LoadArchive, 0, 0, 0, 0, },
	{ 10, 0, 0L, 0L, 0L, NULL, NULL, "ArchiveCancelBtn", 2009, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_ArchiveLog[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ArchiveLogWnd", 1012, RealizeArchiveWnd, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "ArchiveLogWnd", 1013, UnrealizeArchiveWnd, 0, 0, 0, 0, },
	{ 8, 3, 0L, 0L, 0L, NULL, NULL, "ArchiveLogWnd", 68000, (int(*)(PtWidget_t*,ApInfo_t*,PtCallbackInfo_t*)) draw_log_item, 0, 0, 0, 0, },
	{ 4, 0, 0L, 0L, 0L, &ArchiveDlg, NULL, "ArchiveLogOpenBtn", 2009, NULL, 0, 9, 0, 0, },
	{ 10, 0, 0L, 0L, 0L, NULL, NULL, "ArchiveLogCancelBtn", 2009, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_EscalatorMenu[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "MoveUp", 2009, MoveUp, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "MoveStop", 2009, MoveStop, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "MoveDown", 2009, MoveDown, 0, 0, 0, 0, },
	{ 0 }
	};


#if defined(__cplusplus)
}
#endif
