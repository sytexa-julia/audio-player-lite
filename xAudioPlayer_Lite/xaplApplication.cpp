#include "wx/image.h"
#include "wx/fs_mem.h"
#include "xaplApplication.h"
#include "xAudioInterface.h"
#include "mswAudioInterface.h"

bool xaplApplication::OnInit( void )
{
	xAudioInterface *audioInterface = new mswAudioInterface;
	
	// Extra wxWidgets setup
	wxImage::AddHandler( new wxPNGHandler() );
	wxFileSystem::AddHandler( new wxMemoryFSHandler() );
	
	// Create and show main window
	ui = new xaplMainWindow( audioInterface, wxPoint( 50, 50 ) );
	ui->Show( true );
    
	SetTopWindow( ui );

	return true;
}

int xaplApplication::FilterEvent( wxEvent &evt )
{
	wxKeyEvent *keyEvt = wxDynamicCast( &evt, wxKeyEvent );
	if ( keyEvt != NULL )
	{
		if ( keyEvt->GetEventObject() != &dummy )
		{
			keyEvt->SetEventObject( &dummy );
			::wxPostEvent( ui, *keyEvt );
		}
	}

	return -1;
}
