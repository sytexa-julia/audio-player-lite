#ifndef __XAPL_FILE_DROP_TARGET_H
#define __XAPL_FILE_DROP_TARGET_H

#include <wx/wx.h>
#include <wx/dnd.h>
#include "xaplPlaylistManager.h"

class xaplFileDropTarget :
	public wxFileDropTarget
{
private:
	xaplPlaylistManager *m_plManager;

public:
	xaplFileDropTarget( xaplPlaylistManager *plManager );
	virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString &files );
};

#endif
