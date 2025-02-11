#include "xaplFileDropTarget.h"

xaplFileDropTarget::xaplFileDropTarget( xaplPlaylistManager *plManager ) : wxFileDropTarget()
{
	this->m_plManager = plManager;
}

bool xaplFileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString &files )
{
	if ( m_plManager != NULL )
	{
		m_plManager->AddFiles( files );
		return true;
	}

	return false;
}
