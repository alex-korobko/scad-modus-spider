/* Link header for application - AppBuilder 2.01  */

extern ApContext_t AbContext;

ApWindowLink_t base = {
	"base.wgtw",
	&AbContext,
	AbLinks_base, 0, 8
	};

ApPictureLink_t resources = {
	"resources.wgtp",
	&AbContext,
	NULL,
	4, 0
	};

static ApItem_t ApItems_file_menu[ 4 ] = {
	{ 1, 1, 1,"O", 0, "file_open", "Open", NULL },
	{ 1, 16, 0, NULL, 4, "", "", NULL },
	{ 1, 1, 1,"x", 0, "file_exit", "Exit", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t file_menu = {
	"file_menu",
	"",
	NULL,
	NULL,
	-2,
	ApItems_file_menu,
	& AbContext,
	AbLinks_file_menu,
	7, 2, 3
	};

static ApItem_t ApItems_help_menu[ 2 ] = {
	{ 1, 1, 0, NULL, 0, "help_about", "About", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t help_menu = {
	"help_menu",
	"",
	NULL,
	NULL,
	-2,
	ApItems_help_menu,
	& AbContext,
	NULL,
	11, 0, 1
	};

static ApItem_t ApItems_options_menu[ 3 ] = {
	{ 1, 64, 0, NULL, 0, "block_names", "Blocking names (fields separator \\t)", NULL },
	{ 1, 64, 0, NULL, 0, "journal", "Journal (slogger format)", NULL },
	{ 0, 0, NULL, NULL, 0, NULL, NULL, NULL } };

ApMenuLink_t options_menu = {
	"options_menu",
	"",
	NULL,
	NULL,
	-2,
	ApItems_options_menu,
	& AbContext,
	AbLinks_options_menu,
	13, 2, 2
	};

