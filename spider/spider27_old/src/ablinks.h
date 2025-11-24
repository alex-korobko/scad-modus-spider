/* Link header for application - AppBuilder 2.01  */

#if defined(__cplusplus)
extern "C" {
#endif

extern ApContext_t AbContext;

ApWindowLink_t main_wnd = {
	"main_wnd.wgtw",
	&AbContext,
	AbLinks_main_wnd, 0, 10
	};

ApPictureLink_t interface = {
	"interface.wgtp",
	&AbContext,
	AbLinks_interface,
	14, 3
	};

ApDialogLink_t LogFilterDlg = {
	"LogFilterDlg.wgtd",
	&AbContext,
	AbLinks_LogFilterDlg, 96, 9
	};

ApDialogLink_t DeviceConfig = {
	"DeviceConfig.wgtd",
	&AbContext,
	AbLinks_DeviceConfig, 116, 13
	};

ApDialogLink_t SignalDlg = {
	"SignalDlg.wgtd",
	&AbContext,
	AbLinks_SignalDlg, 130, 2
	};

ApDialogLink_t ArchiveDlg = {
	"ArchiveDlg.wgtd",
	&AbContext,
	AbLinks_ArchiveDlg, 141, 2
	};

ApWindowLink_t ArchiveLog = {
	"ArchiveLog.wgtw",
	&AbContext,
	AbLinks_ArchiveLog, 145, 6
	};

ApDialogLink_t PrevCmdPoolDlg = {
	"PrevCmdPoolDlg.wgtd",
	&AbContext,
	AbLinks_PrevCmdPoolDlg, 150, 5
	};

static ApItem_t ApItems_EscalatorMenu[ 4 ] = {
	{ 1, 1, 0, NULL, 0, "MoveUp", "\xD0\x92\xD0\xB2\xD0\xB5\xD1\x80\xD1\x85", NULL },
	{ 1, 1, 0, NULL, 0, "MoveStop", "\xD0\x9E\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD0\xBE\xD0\xB2\xD0\xB8\xD1\x82\xD1\x8C", NULL },
	{ 1, 1, 0, NULL, 0, "MoveDown", "\xD0\x92\xD0\xBD\xD0\xB8\xD0\xB7", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t EscalatorMenu = {
	"EscalatorMenu",
	"",
	"Caption09b",
	"Caption09",
	-2,
	ApItems_EscalatorMenu,
	& AbContext,
	AbLinks_EscalatorMenu,
	155, 3, 3
	};


#if defined(__cplusplus)
}
#endif

