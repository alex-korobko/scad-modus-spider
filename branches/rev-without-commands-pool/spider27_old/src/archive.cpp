/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "global.h"
#include "abimport.h"
#include "proto.h"

int RealizeArchiveWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	g_archiveLog.AttachWnd(widget);
	return( Pt_CONTINUE );
}


int UnrealizeArchiveWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	g_archiveLog.DetachWnd();
	return( Pt_CONTINUE );
}


int LoadArchive( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtFileSelItem_t*	item;
	
	item = PtFSGetCurrent(ABW_ArchiveFiles);
	
	if (item)
	{
		g_archiveLog.Clear();
		g_archiveLog.Load(item->fullpath);
	}

	return( Pt_CONTINUE );
}

