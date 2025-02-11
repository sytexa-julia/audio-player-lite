#include "xaplPlaylistCtrl.h"

BEGIN_EVENT_TABLE(xaplPlaylistCtrl, wxListCtrl)
	EVT_SIZE(xaplPlaylistCtrl::OnSize)
END_EVENT_TABLE()

xaplPlaylistCtrl::xaplPlaylistCtrl( wxWindow* parent, wxWindowID id ) : wxListCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL | wxLC_REPORT | wxLC_NO_HEADER )
{
	InsertColumn( 0, wxT("Playlist Items") );
	SetColumnWidth( 0, -1 );
}

void xaplPlaylistCtrl::SetSelection( long item )
{
	SetItemState( item, wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED );
}

long xaplPlaylistCtrl::GetSelection( void )
{
	// Control is Single Selection only, so this is valid
	return GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
}

wxString xaplPlaylistCtrl::GetStringSelection( void )
{
	// Control is Single Selection only, so this is valid
	long sel = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	return GetItemText( sel );
}

void xaplPlaylistCtrl::InsertItems( const wxArrayString &items, long pos )
{
	for ( size_t i = 0; i < items.Count(); ++i )
	{
		InsertItem( xaplPlaylistItem( items.Item(i) ) );
	}
}

void xaplPlaylistCtrl::OnSize( wxSizeEvent &evt )
{
	// To hide the horizontal scroll bar, we have to subtract 4 on Windows Vista
	// I suspect the '4' might change depending on platform and platform version...
	// There is probably a better way to disable the horizontal scroll bar
	SetColumnWidth( 0, evt.GetSize().GetWidth() - 4 );
	evt.Skip(true);
}
