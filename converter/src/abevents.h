/* Event header for application - AppBuilder 2.01  */

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &base, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

ApEventLink_t AbInternalLinks[] = {
	{ 11, 0, 0L, 0L, 0L, &resources, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &options_menu, NULL, NULL, 0, NULL, 0, 1, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_base[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base", 1012, releazed_app, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "base", 1009, quit_app, 0, 0, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &file_menu, NULL, "file_base", 2007, NULL, 0, 1, 0, 0, },
	{ 5, 2, 102L, 4L, 0L, &file_menu, NULL, "file_base", 1010, NULL, 0, 1, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &options_menu, NULL, "options_base", 2007, NULL, 0, 1, 0, 0, },
	{ 5, 2, 111L, 4L, 0L, &options_menu, NULL, "options_base", 1010, NULL, 0, 1, 0, 0, },
	{ 5, 0, 0L, 0L, 0L, &help_menu, NULL, "help_base", 2007, NULL, 0, 1, 0, 0, },
	{ 5, 2, 104L, 4L, 0L, &help_menu, NULL, "help_base", 1010, NULL, 0, 1, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_file_menu[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_open", 2009, main_WndOpenFile, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "file_exit", 2009, quit_app, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_options_menu[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "block_names", 2009, switch_options, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "journal", 2009, switch_options, 0, 0, 0, 0, },
	{ 0 }
	};

const char ApOptions[] = AB_OPTIONS;
