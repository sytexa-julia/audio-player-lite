#ifndef __XAPL_PLAYLIST_CTRL_H
#define __XAPL_PLAYLIST_CTRL_H

#include <wx/listctrl.h>
#include "xaplPlaylistItem.h"

class xaplPlaylistCtrl :
	public wxListCtrl
{
public:
	xaplPlaylistCtrl( wxWindow* parent, wxWindowID id );
	void SetSelection( long item );
	long GetSelection( void );
	wxString GetStringSelection( void );
	void InsertItems( const wxArrayString &items, long pos );
	void OnSize( wxSizeEvent &evt );

	DECLARE_EVENT_TABLE();
};

#endif
