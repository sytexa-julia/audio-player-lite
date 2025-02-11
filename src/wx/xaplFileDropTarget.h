#ifndef __XAPL_FILE_DROP_TARGET_H
#define __XAPL_FILE_DROP_TARGET_H

#include <wx/wx.h>
#include <wx/dnd.h>
#include "xaplPlaylistCtrl.h"

class xaplFileDropTarget :
	public wxFileDropTarget
{
private:
	xaplPlaylistCtrl *m_plControl;

public:
	xaplFileDropTarget( xaplPlaylistCtrl *plControl );
	virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString &files );
};

#endif
