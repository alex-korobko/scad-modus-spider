/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01A */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include <assert.h>
#include "global.h"
#include "abimport.h"
#include "proto.h"
#include "system.h"


extern metro_escalator* curEscalator;
extern metro_escalator* control;
void fill_signal_blocks(int type_num);

int realize_log_wnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
        PtSetResource(widget, Pt_ARG_TOP_ITEM_POS, 2, 0);

        /* eliminate 'unreferenced' warnings */
        widget = widget, apinfo = apinfo, cbinfo = cbinfo;

        return( Pt_CONTINUE );
}
int
select_escalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

	}

