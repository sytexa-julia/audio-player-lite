#include "xaplIPC.h"
#include "xaplApplication.h"
#include "xaplPlaylistWindow.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplApplication::~xaplApplication( void )
{
	delete m_cfg;
	delete m_cfgFile;
	delete m_checker;

	if ( m_ipcServerRunning )
		delete m_ipcServer;
}

bool xaplApplication::OnInit( void )
{		
	SetAppName( wxT("Xapl") );
	
	if ( !wxDir::Exists( CONFIG_DIR ) )
		wxFileName::Mkdir( CONFIG_DIR );
	
	// Seed random number generator (for shuffle mode)
	srand( xaplUtilities::GetTimeSeed() ); 

	// Extra wxWidgets setup
	wxStandardPaths sp;
    wxString path = sp.GetExecutablePath();
    wxSetWorkingDirectory(path); 

	wxImage::AddHandler( new wxPNGHandler() );
	wxFileSystem::AddHandler( new wxMemoryFSHandler() );

	// Load config file
	m_cfg = new xaplConfiguration;
	LoadConfig();

	// Check for single instance (per user)
	m_ipcServerRunning = false;
	m_checker = new wxSingleInstanceChecker( wxString::Format( wxT("Xapl-%s"), wxGetUserId().c_str() ) );
	if ( m_cfg->singleInstance )
	{
		if ( m_checker->IsAnotherRunning() )
		{
			wxLogNull logNull;

			xaplClient *client = new xaplClient;
			wxConnectionBase* connection = client->MakeConnection( wxT("localhost"), wxT("xapl"), wxT("xapl") );

			if ( connection )
			{
				wxString argvstr;
				if ( wxApp::argc > 1 )  // Append files to playlist and raise the other instance.
				{
					for ( int i = 1; i < wxApp::argc; ++i )
						argvstr << wxApp::argv[i] << wxT("?");

					connection->Execute( argvstr );
				}
				else  // Ask the other instance to raise itself.
				{
					connection->Execute( wxEmptyString );
				}

				connection->Disconnect();
				delete connection;
			}

			delete client;
			SaveConfig();
			return false;
		}
		else
		{
			MakeSingleInstance( true );
		}
	}

	// Create and show main window
	m_ui = new xaplPlaylistWindow;
	if ( !(m_cfg->minToTray && m_cfg->windowMinimized) ) m_ui->Show();
    
	SetTopWindow( m_ui );

	// Add files from command line (and start playing the first added)
	if ( wxApp::argc > 1 )
	{
		wxArrayString files;
		for ( int i = 1; i < wxApp::argc; ++i )
			files.Add( wxApp::argv[i] );

		m_ui->playlist->AddItems( files );
	}

	return true;
}

void xaplApplication::Exit( void )
{
	if ( m_ui != NULL )
		m_ui->Close( true );
	else
		ExitMainLoop();
}

int xaplApplication::OnExit( void )
{
	SaveConfig();
	return 0;
}

int xaplApplication::FilterEvent( wxEvent &evt )
{
	// Catch all key events for custom shortcuts and quick-search
	wxKeyEvent *keyEvt = wxDynamicCast( &evt, wxKeyEvent );
	if ( keyEvt != NULL )
	{
		if ( keyEvt->GetEventObject() != &m_dummy )
		{
			keyEvt->SetEventObject( &m_dummy );
			wxPostEvent( m_ui, *keyEvt );
		}
	}

	// Forward all scroll events to the playlist
	wxMouseEvent *mouseEvt = wxDynamicCast( &evt, wxMouseEvent );
	if ( mouseEvt != NULL )
	{
		if ( mouseEvt->GetEventType() == wxEVT_MOUSEWHEEL && mouseEvt->GetEventObject() != &m_dummy )
		{
			mouseEvt->SetEventObject( &m_dummy );
			if ( m_ui->IsActive() ) wxPostEvent( m_ui->playlist, *mouseEvt );
			return true;
		}
	}

	return -1;
}

void xaplApplication::LoadConfig( void )
{
	// Create m_cfg file if it does not exist
	wxString confFile = CONFIG_FILE;
	const wxChar *c_m_cfgFile = confFile.c_str();
	if ( !wxFile::Exists( c_m_cfgFile ) )
	{
		wxFile file;
		file.Create( c_m_cfgFile );
	}

	wxFFileInputStream stream( CONFIG_FILE );
	m_cfgFile = new wxFileConfig( stream );

	//////////////////////////////////////////////////////
	// File Handling
	//////////////////////////////////////////////////////

	m_cfgFile->Read( wxT("types/prevMp3Assoc"), &m_cfg->prevMp3Assoc, wxEmptyString );
	m_cfgFile->Read( wxT("types/prevOggAssoc"), &m_cfg->prevOggAssoc, wxEmptyString );
	m_cfgFile->Read( wxT("types/prevWavAssoc"), &m_cfg->prevWavAssoc, wxEmptyString );
	m_cfgFile->Read( wxT("types/prevWmaAssoc"), &m_cfg->prevWmaAssoc, wxEmptyString );
	m_cfgFile->Read( wxT("types/prevAacAssoc"), &m_cfg->prevAacAssoc, wxEmptyString );
	m_cfgFile->Read( wxT("types/prevFlacAssoc"), &m_cfg->prevFlacAssoc, wxEmptyString );

	m_cfgFile->Read( wxT("types/mp3"), &m_cfg->mp3, false );
	m_cfgFile->Read( wxT("types/ogg"), &m_cfg->ogg, false );
	m_cfgFile->Read( wxT("types/wav"), &m_cfg->wav, false );
	m_cfgFile->Read( wxT("types/wma"), &m_cfg->wma, false );
	m_cfgFile->Read( wxT("types/aac"), &m_cfg->aac, false );
	m_cfgFile->Read( wxT("types/flac"), &m_cfg->flac, false );

	//////////////////////////////////////////////////////
	// UI State
	//////////////////////////////////////////////////////

	// Playlist state
	m_cfgFile->Read( wxT("ui/plVisible"), &m_cfg->plVisible, true );

	// Window size
	int dispWidth, dispHeight;
	wxDisplaySize( &dispWidth, &dispHeight );
	long width = 0, height = 0;
	if ( m_cfgFile->Read( wxT("ui/winWidth"), &width ) && m_cfgFile->Read( wxT("ui/winHeight"), &height ) )
	{
		// Don't want to be bigger than the screen
		if ( width > dispWidth )
			width = dispWidth;
		if ( height > dispHeight )
			height = dispHeight;
		
		if ( m_cfg->plVisible )
			m_cfg->windowSize.Set( width, height );
		else
			m_cfg->windowSize.Set( 340, 96 );
	}
	else
	{
		if ( m_cfg->plVisible )
			m_cfg->windowSize.Set( 340, 400 );
		else
			m_cfg->windowSize.Set( 340, 96 );
	}

	// Window position
	long xp, yp;
	if ( m_cfgFile->Read( wxT("ui/winX"), &xp, 1 ) && m_cfgFile->Read( wxT("ui/winY"), &yp, 1 ) && xp >= 0 && yp >= 0 )
	{
		// Make sure we don't go off the screen
		if ( xp > dispWidth )
			xp = dispWidth - width;
		if ( yp > dispHeight )
			yp = dispHeight - height;
		
		m_cfg->windowPosition.x = xp;
		m_cfg->windowPosition.y = yp;
	}
	else
	{
		m_cfg->windowPosition.x = 1;
		m_cfg->windowPosition.y = 1;
	}

	// Window minimized
	m_cfgFile->Read( wxT("ui/min"), &m_cfg->windowMinimized, false );

	// Window maximized
	m_cfgFile->Read( wxT("ui/max"), &m_cfg->windowMaximized, false );

	// Window transparency
#ifdef __WINDOWS__
	if ( m_cfgFile->Read( wxT("ui/trans"), &m_cfg->windowTransparency, 255 ) )
	{
		m_cfg->windowTransparency = m_cfg->windowTransparency > 255 ? 255 : m_cfg->windowTransparency;
		m_cfg->windowTransparency = m_cfg->windowTransparency < 25 ? 25 : m_cfg->windowTransparency;
	}
#endif

	// Search visible
	m_cfgFile->Read( wxT("ui/searchVisible"), &m_cfg->searchVisible, false );

	// Search terms
	m_cfgFile->Read( wxT("ui/searchTerms"), &m_cfg->searchTerms, wxEmptyString );

	// Single instance
	m_cfgFile->Read( wxT("ui/singleInst"), &m_cfg->singleInstance, true );

	// Always on top?
	m_cfgFile->Read( wxT("ui/alwaysOnTop"), &m_cfg->alwaysOnTop, false );

	// Minimize to tray?
#ifdef __WINDOWS__
	m_cfgFile->Read( wxT("ui/minToTray"), &m_cfg->minToTray, true );
#else		// default to false on non-windows systems
	m_cfgFile->Read( wxT("ui/minToTray"), &m_cfg->minToTray, false );
#endif

	//////////////////////////////////////////////////////
	// Application State
	//////////////////////////////////////////////////////

	// Playlist format
	m_cfgFile->Read( wxT("state/plFormat"), &m_cfg->plFormat, PLFORMAT_FILENAME );

	// Playback mode
	m_cfgFile->Read( wxT("state/playMode"), &m_cfg->playMode, PLAYMODE_LOOP_LIST );

	// Softmute mode
	m_cfgFile->Read( wxT("state/softMute"), &m_cfg->softMute, false );

	// Volume
	if ( m_cfg->softMute )
		m_cfg->volume = 30;
	else
		m_cfgFile->Read( wxT("state/volume"), &m_cfg->volume, 100 );

	// Loaded playlist
	m_cfgFile->Read( wxT("state/pl"), &m_cfg->loadedPlaylist, wxString::Format( wxT("%s%s"), CONFIG_DIR.c_str(), wxT("Default.m3u") ) );

	// Selected song
	m_cfgFile->Read( wxT("state/song"), &m_cfg->selectedFile, 0 );

	// Last opened directory
	m_cfgFile->Read( wxT("state/lastDir"), &m_cfg->lastOpenedDir, STANDARD_PATHS.GetDocumentsDir() );

	// Auto delete bad items on playlist save?
	m_cfgFile->Read( wxT("state/autoDeleteInvalid"), &m_cfg->autoDeleteInvalid, true );

	//////////////////////////////////////////////////////
	// Audio Settings
	//////////////////////////////////////////////////////

	// Buffer size
	m_cfgFile->Read( wxT("audio/bufferSize"), &m_cfg->bufferSize, 3000 );

	// Force software mixing
	m_cfgFile->Read( wxT("audio/forceSoftware"), &m_cfg->forceSoftwareMixing, false );

	//////////////////////////////////////////////////////
	// Playlist Colors
	//////////////////////////////////////////////////////

	// Playlist style
	wxString bg, fg, selectionBg, selectionFg, queuedBg, queuedFg, nowPlayingBg, nowPlayingFg;
	m_cfgFile->Read( wxT("style/bg"), &bg, wxT("#FFFFFF") );
	m_cfgFile->Read( wxT("style/fg"), &fg, wxT("#000000") );
	m_cfgFile->Read( wxT("style/selbg"), &selectionBg, wxT("#3399FF") );
	m_cfgFile->Read( wxT("style/selfg"), &selectionFg, wxT("#FFFFFF") );
	m_cfgFile->Read( wxT("style/qbg"), &queuedBg, wxT("#FFFFFF") );
	m_cfgFile->Read( wxT("style/qfg"), &queuedFg, wxT("#FF9900") );
	m_cfgFile->Read( wxT("style/npbg"), &nowPlayingBg, wxT("#FFFFFF") );
	m_cfgFile->Read( wxT("style/npfg"), &nowPlayingFg, wxT("#000000") );
	
	m_cfg->plStyle.font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
	m_cfg->plStyle.bg = wxColour( bg );
	m_cfg->plStyle.fg = wxColour( fg );
	m_cfg->plStyle.selectionBg = wxColour( selectionBg );
	m_cfg->plStyle.selectionFg = wxColour( selectionFg );
	m_cfg->plStyle.queuedBg = wxColour( queuedBg );
	m_cfg->plStyle.queuedFg = wxColour( queuedFg );
	m_cfg->plStyle.nowPlayingBg = wxColour( nowPlayingBg );
	m_cfg->plStyle.nowPlayingFg = wxColour( nowPlayingFg );

	//////////////////////////////////////////////////////
	// Update Directories
	//////////////////////////////////////////////////////

	size_t pos = 1;
	wxString dir;
	while ( m_cfgFile->Read( wxString::Format( wxT("dirs/directory%d"), pos++ ), &dir ) )
		m_cfg->mediaDirs.Add( dir );
}

void xaplApplication::SaveConfig( void )
{
	// File handling
	m_cfgFile->Write( wxT("types/prevMp3Assoc"), m_cfg->prevMp3Assoc );
	m_cfgFile->Write( wxT("types/prevOggAssoc"), m_cfg->prevOggAssoc );
	m_cfgFile->Write( wxT("types/prevWavAssoc"), m_cfg->prevWavAssoc );
	m_cfgFile->Write( wxT("types/prevWmaAssoc"), m_cfg->prevWmaAssoc );
	m_cfgFile->Write( wxT("types/prevAacAssoc"), m_cfg->prevAacAssoc );
	m_cfgFile->Write( wxT("types/prevFlacAssoc"), m_cfg->prevFlacAssoc );

	m_cfgFile->Write( wxT("types/mp3"), m_cfg->mp3 );
	m_cfgFile->Write( wxT("types/ogg"), m_cfg->ogg );
	m_cfgFile->Write( wxT("types/wav"), m_cfg->wav );
	m_cfgFile->Write( wxT("types/wma"), m_cfg->wma );
	m_cfgFile->Write( wxT("types/aac"), m_cfg->aac );
	m_cfgFile->Write( wxT("types/flac"), m_cfg->flac );

	// UI state
	m_cfgFile->Write( wxT("ui/plVisible"), m_cfg->plVisible );
	m_cfgFile->Write( wxT("ui/winWidth"), m_cfg->windowSize.GetWidth() );
	m_cfgFile->Write( wxT("ui/winHeight"), m_cfg->windowSize.GetHeight() );
	if ( m_cfg->windowPosition.x != -32000 && m_cfg->windowPosition.y != -32000 )
	{
		m_cfgFile->Write( wxT("ui/winX"), m_cfg->windowPosition.x );
		m_cfgFile->Write( wxT("ui/winY"), m_cfg->windowPosition.y );
	}
	m_cfgFile->Write( wxT("ui/min"), m_cfg->windowMinimized );
	m_cfgFile->Write( wxT("ui/max"), m_cfg->windowMaximized );
#ifdef __WINDOWS__
	m_cfgFile->Write( wxT("ui/trans"), m_cfg->windowTransparency );
#endif
	m_cfgFile->Write( wxT("ui/searchVisible"), m_cfg->searchVisible );
	m_cfgFile->Write( wxT("ui/searchTerms"), m_cfg->searchTerms );
	m_cfgFile->Write( wxT("ui/singleInst"), m_cfg->singleInstance );
	m_cfgFile->Write( wxT("ui/alwaysOnTop"), m_cfg->alwaysOnTop );
	m_cfgFile->Write( wxT("ui/minToTray"), m_cfg->minToTray );

	// Application state
	m_cfgFile->Write( wxT("state/plFormat"), m_cfg->plFormat );
	m_cfgFile->Write( wxT("state/playMode"), m_cfg->playMode );
	m_cfgFile->Write( wxT("state/softMute"), m_cfg->softMute );
	m_cfgFile->Write( wxT("state/volume"), m_cfg->volume );
	m_cfgFile->Write( wxT("state/pl"), m_cfg->loadedPlaylist );
	m_cfgFile->Write( wxT("state/song"), m_cfg->selectedFile );
	m_cfgFile->Write( wxT("state/lastDir"), m_cfg->lastOpenedDir );
	m_cfgFile->Write( wxT("state/autoDeleteInvalid"), m_cfg->autoDeleteInvalid );

	// Audio settings
	m_cfgFile->Write( wxT("audio/bufferSize"), m_cfg->bufferSize );
	m_cfgFile->Write( wxT("audio/forceSoftware"), m_cfg->forceSoftwareMixing );
	
	// Style
	m_cfgFile->Write( wxT("style/bg"), m_cfg->plStyle.bg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/fg"), m_cfg->plStyle.fg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/selbg"), m_cfg->plStyle.selectionBg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/selfg"), m_cfg->plStyle.selectionFg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/qbg"), m_cfg->plStyle.queuedBg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/qfg"), m_cfg->plStyle.queuedFg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/npbg"), m_cfg->plStyle.nowPlayingBg.GetAsString(wxC2S_HTML_SYNTAX) );
	m_cfgFile->Write( wxT("style/npfg"), m_cfg->plStyle.nowPlayingFg.GetAsString(wxC2S_HTML_SYNTAX) );

	// Update directories
	for ( size_t i = 1; i <= m_cfg->mediaDirs.GetCount(); ++i )
		m_cfgFile->Write( wxString::Format( wxT("dirs/directory%d"), i ), m_cfg->mediaDirs[i - 1] );

	// Create m_cfg file if it does not exist
	const wxChar *c_m_cfgFile = CONFIG_FILE.c_str();
	if ( !wxFile::Exists( c_m_cfgFile ) )
	{
		wxFile file;
		file.Create( c_m_cfgFile );
	}

	// Try to save m_cfgFile
	wxFFileOutputStream stream( CONFIG_FILE );
	if ( !m_cfgFile->Save( stream ) )
		wxMessageBox( wxT("Warning: Could not save m_cfgFile file!") );
	stream.Close();
}

void xaplApplication::MakeSingleInstance( bool m )
{
	if ( m && !m_ipcServerRunning )
	{
		m_ipcServer = new xaplServer;
		if ( !m_ipcServer->Create(wxT("xapl") ) )
		{
			wxLogDebug( wxT("Failed to create an IPC service.") );
		}
		m_ipcServerRunning = true;
	}
	else
	{
		if ( m_ipcServerRunning )
		{
			delete m_ipcServer;
			m_ipcServerRunning = false;
		}
	}
}
