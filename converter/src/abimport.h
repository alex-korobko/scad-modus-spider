/* Import (extern) header for application - AppBuilder 2.01  */

#include "abdefine.h"

extern ApEventLink_t AbInternalLinks[];

extern ApWindowLink_t base;
extern ApPictureLink_t resources;
extern ApWidget_t AbWidgets[ 7 ];

extern ApMenuLink_t file_menu;
extern ApMenuLink_t help_menu;
extern ApMenuLink_t options_menu;

#ifdef __cplusplus
extern "C" {
#endif
int quit_app( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int main_WndOpenFile( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int switch_options( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int releazed_app( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
