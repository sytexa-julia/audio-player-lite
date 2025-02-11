#include "xAutoUpdaterApp.h"

bool xAutoUpdaterApp::OnInit( void )
{
	// Zip handler
	wxFileSystem::AddHandler( new wxZipFSHandler ); 

	// Read info from command line
	if ( wxApp::argc > 1 )
	{
		wxArrayString arguments;
		wxString productName, localVersion, broadcastPage, installLocation, mainExe, updateMode;

		for ( int i = 1; i < wxApp::argc; ++i )
			arguments.Add( wxApp::argv[i] );

		if ( arguments.GetCount() < 5 )
			return false;

		productName = arguments[0].Trim().Trim( false );
		localVersion = arguments[1].Trim().Trim( false );
		broadcastPage = arguments[2].Trim().Trim( false );
		installLocation = arguments[3].Trim().Trim( false );
		mainExe = arguments[4].Trim().Trim( false );

		if ( arguments.GetCount() > 5 ) 
		{
			updateMode = arguments[5].Trim().Trim( false );
			long um;
			if ( updateMode.ToLong( &um ) )
				m_updateCheck = new xUpdateCheck( productName, localVersion, broadcastPage, installLocation, mainExe, (int) um );
			else
				m_updateCheck = new xUpdateCheck( productName, localVersion, broadcastPage, installLocation, mainExe );
		}
		else
		{
			m_updateCheck = new xUpdateCheck( productName, localVersion, broadcastPage, installLocation, mainExe );
		}

		if ( m_updateCheck != NULL )
			m_updateCheck->PerformCheck();

		delete m_updateCheck;

		return false;
	}

	return false;
}
