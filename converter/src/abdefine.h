/* Define header for application - AppBuilder 2.01  */

/* Internal Module Links */
extern const int ABI_resources;
#define ABM_resources                        &AbInternalLinks[ABI_resources]
extern const int ABI_options_menu;
#define ABM_options_menu                     &AbInternalLinks[ABI_options_menu]

/* 'base' Window link */
extern const int ABN_base;
#define ABW_base                             AbGetABW( ABN_base )
extern const int ABN_file_base;
#define ABW_file_base                        AbGetABW( ABN_file_base )
extern const int ABN_options_base;
#define ABW_options_base                     AbGetABW( ABN_options_base )
extern const int ABN_help_base;
#define ABW_help_base                        AbGetABW( ABN_help_base )

/* 'resources' Picture link */
extern const int ABN_resources;
#define ABW_resources                        AbGetABW( ABN_resources )
extern const int ABN_options_on;
#define ABW_options_on                       AbGetABW( ABN_options_on )
extern const int ABN_options_off;
#define ABW_options_off                      AbGetABW( ABN_options_off )

#define AbGetABW( n ) ( AbWidgets[ n ].wgt )

/* 'file_menu' Menu link */
extern const int ABN_file_menu;
extern const int ABN_file_open;
extern const int ABN_file_exit;

/* 'help_menu' Menu link */
extern const int ABN_help_menu;
extern const int ABN_help_about;

/* 'options_menu' Menu link */
extern const int ABN_options_menu;
extern const int ABN_block_names;
extern const int ABN_journal;

#define AB_OPTIONS "s:x:y:h:w:S:"
