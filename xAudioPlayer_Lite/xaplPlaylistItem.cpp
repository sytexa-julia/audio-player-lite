#include "xaplPlaylistItem.h"

xaplPlaylistItem::xaplPlaylistItem( wxString path ) : wxListItem(), filePath( path )
{
	SetText( path );
}

xaplPlaylistItem::xaplPlaylistItem( wxString path, wxString display ) : wxListItem(), filePath( path )
{
	SetText( display );
}

wxString xaplPlaylistItem::GetFilePath( void )
{
	return filePath;
}
