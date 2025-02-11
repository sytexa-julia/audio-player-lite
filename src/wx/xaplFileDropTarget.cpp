#include "xaplFileDropTarget.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplFileDropTarget::xaplFileDropTarget( xaplPlaylistCtrl *plControl ) : wxFileDropTarget(), m_plControl( plControl )
{}

bool xaplFileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString &files )
{
		if ( m_plControl != NULL )
		{
			m_plControl->AddItems( files );
			return true;
		}

	return false;
}
