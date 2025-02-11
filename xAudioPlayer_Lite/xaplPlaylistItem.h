#ifndef __XAPL_PLAYLIST_ITEM_H
#define __XAPL_PLAYLIST_ITEM_H

#include <wx/listctrl.h>

class xaplPlaylistItem :
	public wxListItem
{
private:
	wxString filePath;
public:
	xaplPlaylistItem( wxString path );
	xaplPlaylistItem( wxString path, wxString display );
	wxString GetFilePath( void );
};

#endif
