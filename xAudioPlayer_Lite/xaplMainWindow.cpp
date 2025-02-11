#include <wx/gbsizer.h>
#include <wx/wfstream.h>
#include "xaplMainWindow.h"
#include "xaplConstants.h"
#include "xaplFileDropTarget.h"
#include "resource.h"

/* ====================================================================
 * ==== EVENT TABLE DECLARATION =====================================*/

BEGIN_EVENT_TABLE( xaplMainWindow, wxFrame )
	EVT_MENU( MenuItem_File_OpenFile, xaplMainWindow::OnMenuOpenFile )
	EVT_MENU( MenuItem_File_OpenLocation, xaplMainWindow::OnMenuOpenLocation )
	EVT_MENU( MenuItem_File_Exit, xaplMainWindow::OnMenuExit )

	EVT_MENU( MenuItem_View_TogglePlaylist, xaplMainWindow::OnMenuTogglePlaylist )
	EVT_MENU( MenuItem_View_TogglePLFormat, xaplMainWindow::OnMenuTogglePlaylistFormat )
	EVT_MENU( MenuItem_View_PLFileName, xaplMainWindow::OnMenuViewFilename )
	EVT_MENU( MenuItem_View_PLFullPath, xaplMainWindow::OnMenuViewFullPath )
	EVT_MENU( MenuItem_View_PLCustomTags, xaplMainWindow::OnMenuViewCustom )

	EVT_MENU( MenuItem_Play_SoftMute, xaplMainWindow::OnMenuSoftMute )
	EVT_MENU( MenuItem_Play_VolumeUp, xaplMainWindow::OnMenuVolumeUp)
	EVT_MENU( MenuItem_Play_VolumeDown, xaplMainWindow::OnMenuVolumeDown )
	EVT_MENU( MenuItem_Play_Play, xaplMainWindow::OnMenuPlay )
	EVT_MENU( MenuItem_Play_Pause, xaplMainWindow::OnMenuPause )
	EVT_MENU( MenuItem_Play_Stop, xaplMainWindow::OnMenuStop )
	EVT_MENU( MenuItem_Play_Next, xaplMainWindow::OnMenuNextSong )
	EVT_MENU( MenuItem_Play_Prev, xaplMainWindow::OnMenuPrevSong )

	EVT_MENU( MenuItem_Mode_Toggle, xaplMainWindow::OnMenuTogglePlayMode )
	EVT_MENU( MenuItem_Mode_LoopList, xaplMainWindow::OnMenuModeRptList )
	EVT_MENU( MenuItem_Mode_LoopSong, xaplMainWindow::OnMenuModeRptSong )
	EVT_MENU( MenuItem_Mode_Shuffle, xaplMainWindow::OnMenuModeShuffle )

	EVT_MENU( MenuItem_Help_About, xaplMainWindow::OnMenuAbout )

	EVT_LIST_ITEM_ACTIVATED( ID_Playlist, xaplMainWindow::OnPlaylistItemActivated )

	EVT_KEY_DOWN( xaplMainWindow::KeyDown )

	EVT_BUTTON( ID_PlayPauseButton, xaplMainWindow::OnButtonPlayPause )
	EVT_BUTTON( ID_StopButton, xaplMainWindow::OnButtonStop )
	EVT_BUTTON( ID_PrevSongButton, xaplMainWindow::OnButtonPrevSong )
	EVT_BUTTON( ID_NextSongButton, xaplMainWindow::OnButtonNextSong )
END_EVENT_TABLE()

/* ====================================================================
 * ==== (CON|DE)STRUCTORS AND INITIALIZATION ========================*/
xaplMainWindow::xaplMainWindow( xAudioInterface *aIface, const wxPoint& pos ) :
	wxFrame( (wxFrame *)NULL, -1, wxT("xAudio Player Lite"), pos, wxSize(300, 250), wxDEFAULT_FRAME_STYLE ), audioInterface( aIface )
{	
	if ( aIface == NULL )
	{
		wxMessageBox( wxT("Error: Could not initialize audio.") );
		Close( true );
	}

	Initialize();
	LoadConfig();
}

xaplMainWindow::~xaplMainWindow( void )
{
	SaveConfig();

	positionTimer->Pause();
	delete positionTimer;
	
	delete aboutDialog;
	delete controls;
	delete playlist;

	if ( audioInterface != NULL )
		delete audioInterface;
}

void xaplMainWindow::Initialize( void )
{
	// Set the frame icon from the resource file	
	#ifdef __WINDOWS__
	  wxIcon appIco = wxIcon( wxICON( APPICON ) );
	#else
	  wxIcon appIco = wxIcon( app_xpm );
	#endif
	
	if ( appIco.Ok() )
		SetIcon( appIco );
	
	// Set up the menu
	menuBar = new xaplMenuBar;
	SetMenuBar( menuBar );

	// Set up the about dialog
	aboutDialog = new xaplAboutDialog( this );

	// Set up the status bar
	int widths[] = { -2, -2, -4 };
	statusBar = new wxStatusBar( this, wxID_ANY, wxST_SIZEGRIP );
	statusBar->SetFieldsCount( WXSIZEOF(widths) );
	statusBar->SetStatusWidths( WXSIZEOF(widths), widths );

	SetStatusBar( statusBar );

	// Lay out the controls
	controls = new xaplControlUI( this );
	playlist = new xaplPlaylistCtrl( this, ID_Playlist );
	search = new xaplSearchUI( this );

	wxFlexGridSizer *sizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	sizer->Add( controls, 0, wxALIGN_TOP | wxEXPAND );
	sizer->Add( playlist, 0, wxALIGN_TOP | wxEXPAND );
	sizer->Add( search, 0, wxALIGN_TOP | wxEXPAND );

	sizer->AddGrowableCol( 0 );
	sizer->AddGrowableRow( 1 );

	SetSizer( sizer );

	// Misc. setup
	audioInterface->SetCallbackListener( this );

	controls->AddPositionObserver( this );
	controls->AddVolumeObserver( this );
	prevVolume = 1.0f;

	positionTimer = new xaplPlaybackTimer;
	positionTimer->Create();
	positionTimer->Run();
	positionTimer->Pause();
	positionTimer->AddTimeObserver( this );

	plManager = new xaplPlaylistManager;
	this->SetDropTarget( new xaplFileDropTarget( plManager ) );
	
	wxString status;
	status << plManager->GetPlaylistItemCount() << ( plManager->GetPlaylistItemCount() == 1 ? wxT("item") : wxT(" items") );
	statusBar->SetStatusText( status );
}

void xaplMainWindow::LoadConfig( void )
{
	wxString iniPath;
	iniPath << EXECUTABLE_DIR << CONFIG_FILENAME;

	// Create config file if it does not exist
	const wxChar *c_configFile = iniPath.c_str();
	if ( !wxFile::Exists( c_configFile ) )
	{
		wxFile file;
		file.Create( c_configFile );
	}

	wxFFileInputStream stream( iniPath );
	configuration = new wxFileConfig( stream );
	configuration->Create();

	// Playlist state
	bool plVisible = true;
	if ( configuration->Read( wxT("plVisible"), &plVisible ) )
	{
		menuBar->TogglePlaylistVisible( plVisible );
		TogglePlaylist( plVisible );
	}

	// Window size
	long width = 0, height = 0;
	if ( configuration->Read( wxT("winWidth"), &width ) && configuration->Read( wxT("winHeight"), &height ) )
	{
		if ( plVisible )
			SetSize( width, height );
		else
			SetSize( 300, 117 );
	}
	else
	{
		if ( plVisible )
			SetSize( 300, 250 );
		else
			SetSize( 300, 117 );
	}

	// Window position
	long xp, yp;
	if ( configuration->Read( wxT("winX"), &xp ) && configuration->Read( wxT("winY"), &yp ) && xp >= 0 && yp >= 0 )
	{
		SetPosition( wxPoint( xp, yp ) );
	}
	else
	{
		Center();
	}

	// Playlist format
	long plf = PLFORMAT_FILENAME;
	if ( configuration->Read( wxT("plFormat"), &plf ) )
	{
		menuBar->TogglePlaylistFormat( plf );
	}
	else
	{
		plf = PLFORMAT_FILENAME;
		menuBar->TogglePlaylistFormat( plf );
	}

	// Loaded playlist
	wxString currPlaylist;
	if ( configuration->Read( wxT("pl"), &currPlaylist ) )
	{
		if ( !plManager->LoadPlaylist( currPlaylist, plf ) )
			plManager->LoadPlaylist( wxT("Default.xpl"), plf );
	}
	else
	{
		plManager->LoadPlaylist( wxT("Default.xpl"), plf );
	}

	playlist->InsertItems( plManager->GetPlaylist(), 0 );

	// Playback mode
	long pm = PLAYMODE_LOOP_LIST;
	if ( configuration->Read( wxT("playMode"), &pm ) )
	{
		menuBar->TogglePlayMode( pm );
	}

	// Softmute mode
	bool sftmt = false;
	if ( configuration->Read( wxT("softMute"), &sftmt ) )
	{
		menuBar->ToggleSoftMute( sftmt );
	}

	// Volume
	if ( sftmt )
	{
		controls->SetVolume( 0.3f );
		audioInterface->SetVolume( 0.3f );
	}
	else
	{
		double vol;
		if ( configuration->Read( wxT("volume"), &vol ) )
		{
			float fVol = static_cast<float>( vol );
			controls->SetVolume( fVol );
			audioInterface->SetVolume( fVol );
		}
	}

	// Selected song
	long currSong;
	if ( configuration->Read( wxT("song"), &currSong ) )
	{
		long theIndex = (long) currSong;
		if ( theIndex >= 0 && theIndex < playlist->GetItemCount() )
		{
			playlist->SetSelection( theIndex );
		}
	}

	// Search visible
	bool searchVisible = false;
	configuration->Read( wxT("searchVisible"), &searchVisible );
	if ( searchVisible )
		search->Show();
	else
		search->Hide();

	// Search terms
	wxString searchTerms = wxEmptyString;
	if ( configuration->Read( wxT("searchTerms"), &searchTerms ) )
	{
		search->AppendToSearchBox( searchTerms );
		//TODO: Apply filter to playlist...
	}
}

void xaplMainWindow::SaveConfig( void )
{
	configuration->Write( wxT("plVisible"), menuBar->GetPlaylistVisible() );
	configuration->Write( wxT("winWidth"), GetSize().GetWidth() );
	configuration->Write( wxT("winHeight"), GetSize().GetHeight() );
	configuration->Write( wxT("winX"), GetPosition().x );
	configuration->Write( wxT("winY"), GetPosition().y );
	configuration->Write( wxT("plFormat"), menuBar->GetPlaylistFormat() );
	configuration->Write( wxT("formatStr"), menuBar->GetPlaylistFormat() );
	configuration->Write( wxT("playMode"), menuBar->GetPlayMode() );
	configuration->Write( wxT("softMute"), menuBar->GetSoftMuted() );
	configuration->Write( wxT("volume"), static_cast<double>(controls->GetVolume()) );
	configuration->Write( wxT("pl"), plManager->GetActivePlaylistLocation() );
	configuration->Write( wxT("song"), playlist->GetSelection() );
	configuration->Write( wxT("searchVisible"), search->IsShown() );
	configuration->Write( wxT("searchTerms"), search->GetSearchTerms() );

	wxString iniPath;
	iniPath << EXECUTABLE_DIR << CONFIG_FILENAME;

	// Create config file if it does not exist
	const wxChar *c_configFile = iniPath.c_str();
	if ( !wxFile::Exists( c_configFile ) )
	{
		wxFile file;
		file.Create( c_configFile );
	}

	// Try to save configuration
	wxFFileOutputStream stream( iniPath );
	if ( !configuration->Save( stream ) )
		wxMessageBox( wxT("Warning: Could not save configuration file!") );
}

/* ====================================================================
 * ==== NON-WX EVENTS ===============================================*/

void xaplMainWindow::OnChannelEvent( int eventID )
{
	if ( eventID == EVENT_CHANNEL_END )
	{
		switch ( menuBar->GetPlayMode() )
		{
		case PLAYMODE_LOOP_LIST:
			{
				int sel = playlist->GetSelection();
				if ( sel == playlist->GetItemCount() - 1 )
					playlist->SetSelection( 0 );
				else
					playlist->SetSelection( sel + 1 );

				// Load the next song, but only start playing if already playing
				if ( audioInterface->GetState() == STATE_PLAYING )
					OnPlay( playlist->GetStringSelection(), true );
			}
			break;

		case PLAYMODE_SHUFFLE:
			{
				int range = playlist->GetItemCount();
				int sel = rand() % range;
				playlist->SetSelection( sel );

				// Load the next song, but only start playing if already playing
				if ( audioInterface->GetState() == STATE_PLAYING )
					OnPlay( playlist->GetStringSelection(), true );
			}
			break;

		case PLAYMODE_LOOP_SONG:
		default:
				// Just let the song loop
			break;
		}
	}
}

void xaplMainWindow::OnTimeUpdated( void )
{
	controls->SetSongPosition( audioInterface->GetCurrentPosition(), audioInterface->GetStreamLength() );
}

void xaplMainWindow::OnSongPositionChanged( float newPos )
{
	audioInterface->Seek( newPos );
}

void xaplMainWindow::OnVolumeChanged( float newVol )
{
	audioInterface->SetVolume( newVol );
	prevVolume = newVol;

	if ( newVol == 0.3f )
		menuBar->Check( MenuItem_Play_SoftMute, true );
	else
		menuBar->Check( MenuItem_Play_SoftMute, false );
}

void xaplMainWindow::OnPlay( const wxString &audioFile, bool startPlay )
{
	if ( audioFile == wxEmptyString )
		return;

	bool check = audioInterface->LoadFile( audioFile.mb_str(wxConvUTF8), startPlay );
	if ( !check )
	{
		wxMessageBox( wxT("An error occured while loading the specified audio file.") );
	}
	else
	{
		controls->SetSongLength( audioInterface->GetStreamLength() );
		positionTimer->Resume();
	}
}

void xaplMainWindow::KeyDown( wxKeyEvent &evt )
{
	int keyCode = evt.GetKeyCode();
	if ( !evt.ControlDown() && ( (evt.GetKeyCode() >= 65 && evt.GetKeyCode() <= 90) || (evt.GetKeyCode() >= 97 && evt.GetKeyCode() <= 122) ) )
	{
		if ( !search->IsShown() )
		{
			search->Show();
			search->FocusSearchBox();

			if ( evt.ShiftDown() )
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()) );
			else
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()).Lower() );
		}
	}
}

void xaplMainWindow::TogglePlaylist( bool showPL )
{
	if ( showPL )
	{
		playlist->Show();
		SetMinSize( wxSize(300, 250) );
		SetSize( 300, 250 );
		SetWindowStyle( wxDEFAULT_FRAME_STYLE );
	}
	else
	{
		playlist->Hide();
		SetMinSize( wxSize(300, 117) );
		SetSize( 300, 117 );
		SetWindowStyle( wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) );
	}
}

/* ====================================================================
 * ==== WX EVENTS ===================================================*/

//////////////////////////////////////
// Control events
//////////////////////////////////////

void xaplMainWindow::OnPlaylistItemActivated( wxListEvent &evt )
{
	controls->SetPlaying();
	OnPlay( playlist->GetStringSelection() );
}

void xaplMainWindow::OnButtonPlayPause( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( audioInterface->GetState() != STATE_PAUSED && audioInterface->GetState() != STATE_PLAYING )
	{
		controls->TogglePlayPauseIcon();
		OnPlay( playlist->GetStringSelection() );
	}
	else
	{
		OnMenuPause( wxCommandEvent() );
	}
}

void xaplMainWindow::OnButtonStop( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuStop( wxCommandEvent() );
}

void xaplMainWindow::OnButtonPrevSong( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuPrevSong( wxCommandEvent() );
}

void xaplMainWindow::OnButtonNextSong( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuNextSong( wxCommandEvent() );
}

//////////////////////////////////////
// FILE menu
//////////////////////////////////////

void xaplMainWindow::OnMenuOpenFile( wxCommandEvent &WXUNUSED( evt ) )
{
	wxFileDialog *fileDlg = new wxFileDialog( this, wxT("Open Audio Files"), wxEmptyString, wxEmptyString, SUPPORTED_FORMATS, wxOPEN | wxMULTIPLE | wxFILE_MUST_EXIST | wxFD_CHANGE_DIR );
	if ( fileDlg->ShowModal() == wxID_OK )
	{
		// TODO: stick a file on the playlist
	}

	delete fileDlg;
}

void xaplMainWindow::OnMenuOpenLocation( wxCommandEvent &WXUNUSED( evt ) )
{
	wxDirDialog *dirDlg = new wxDirDialog( this, wxT("Open a Directory of Audio Files") );
	if ( dirDlg->ShowModal() == wxID_OK )
	{
		// TODO: stick all playable files in a directory on the playlist
	}

	delete dirDlg;
}


void xaplMainWindow::OnMenuExit( wxCommandEvent &WXUNUSED( evt ) )
{
    Close( true );
}

//////////////////////////////////////
// VIEW menu
//////////////////////////////////////

void xaplMainWindow::OnMenuTogglePlaylist( wxCommandEvent &evt )
{
	TogglePlaylist( evt.IsChecked() );
}

void xaplMainWindow::OnMenuTogglePlaylistFormat( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat();
	if ( plManager->SetPlaylistFormat( menuBar->GetPlaylistFormat() ) )
	{
		playlist->DeleteAllItems();
		playlist->InsertItems( plManager->GetPlaylist(), 0 );
	}
}

void xaplMainWindow::OnMenuViewFilename( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_FILENAME );
	if ( plManager->SetPlaylistFormat( menuBar->GetPlaylistFormat() ) )
	{
		playlist->DeleteAllItems();
		playlist->InsertItems( plManager->GetPlaylist(), 0 );
	}
}

void xaplMainWindow::OnMenuViewFullPath( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_FULLPATH );
	
	if ( plManager->SetPlaylistFormat( menuBar->GetPlaylistFormat() ) )
	{
		playlist->DeleteAllItems();
		playlist->InsertItems( plManager->GetPlaylist(), 0 );
	}
}

void xaplMainWindow::OnMenuViewCustom( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_CUSTOM );
	// Open customization dialog
	wxString plFormatStr = wxEmptyString;
	if ( plManager->SetPlaylistFormat( menuBar->GetPlaylistFormat(), plFormatStr ) )
	{
		playlist->DeleteAllItems();
		playlist->InsertItems( plManager->GetPlaylist(), 0 );
	}
}

//////////////////////////////////////
// CONTROL menu
//////////////////////////////////////

void xaplMainWindow::OnMenuSoftMute( wxCommandEvent &evt )
{
	if ( evt.IsChecked() )
	{
		audioInterface->GetCurrentVolume( &prevVolume );

		// Don't raise the volume if it is already below soft mute level
		if ( prevVolume > 0.3f )
		{
			audioInterface->SetVolume( 0.3f );
			controls->SetVolume( 0.3f );
		}
	}
	else
	{
		audioInterface->SetVolume( prevVolume );
		controls->SetVolume( prevVolume );
	}
}

void xaplMainWindow::OnMenuVolumeDown( wxCommandEvent &evt )
{
	float vol = controls->GetVolume() - 0.1f;

	// Are we at the 'soft mute' volume?
	if ( vol == 0.3f )
		menuBar->ToggleSoftMute( true );
	else
		menuBar->ToggleSoftMute( false );

	audioInterface->SetVolume( vol );
	controls->SetVolume( vol );
}

void xaplMainWindow::OnMenuVolumeUp( wxCommandEvent &evt )
{
	float vol = controls->GetVolume() + 0.1f;

	// Are we at the 'soft mute' volume?
	if ( vol == 0.3f )
		menuBar->ToggleSoftMute( true );
	else
		menuBar->ToggleSoftMute( false );

	audioInterface->SetVolume( vol );
	controls->SetVolume( vol );
}

void xaplMainWindow::OnMenuPlay( wxCommandEvent &evt )
{
	controls->SetPlaying();
	
	if ( audioInterface->GetState() == STATE_PAUSED )
	{
		audioInterface->StartPlayback();
		positionTimer->Resume();
	}
	else
	{
		OnPlay( playlist->GetStringSelection() );
	}
}

void xaplMainWindow::OnMenuPause( wxCommandEvent &evt )
{
	controls->TogglePlayPauseIcon();
	
	// If paused or stopped, resume playback
	int state = audioInterface->GetState();
	if ( state == STATE_PAUSED )
	{
		audioInterface->StartPlayback();
		positionTimer->Resume();
		return;
	}
	else if ( state == STATE_STOPPED || state == STATE_READY ) // the latter condition is for when the app first starts up
	{
		OnPlay( playlist->GetStringSelection() );
		return;
	}

	// Otherwise, pause playback
	audioInterface->PausePlayback();
	positionTimer->Pause();
}

void xaplMainWindow::OnMenuStop( wxCommandEvent &evt )
{
	controls->SetSongLength( 0 );
	controls->SetPaused();
	audioInterface->StopPlayback();
	positionTimer->Pause();
}

void xaplMainWindow::OnMenuNextSong( wxCommandEvent &evt )
{
	switch ( menuBar->GetPlayMode() )
	{
	case PLAYMODE_SHUFFLE:
		// Choose a random next song
		{
			int range = playlist->GetItemCount();

			// If there are more than 20 songs, loop until we find
			// one that isn't a repeat
			int sel = rand() % range;
			while ( playlist->GetItemCount() > 20 && sel == playlist->GetSelection() )
				sel = rand() % range;

			playlist->SetSelection( sel );
		}
		break;

	case PLAYMODE_LOOP_LIST:
	case PLAYMODE_LOOP_SONG:
	default:
		// Skip to the next song
		{
			int sel = playlist->GetSelection();
			if ( sel == playlist->GetItemCount() - 1 )
				playlist->SetSelection( 0 );
			else
				playlist->SetSelection( sel + 1 );
		}
		break;
	}

	// Load the next song and start playing
	controls->SetPlaying();
	OnPlay( playlist->GetStringSelection(), true );
}

void xaplMainWindow::OnMenuPrevSong( wxCommandEvent &evt )
{
	int sel = playlist->GetSelection();

	if ( sel == 0 )
		playlist->SetSelection( playlist->GetItemCount() - 1 );
	else
		playlist->SetSelection( sel - 1 );

	// Load the next song and start playing
	controls->SetPlaying();
	OnPlay( playlist->GetStringSelection(), true );
}

//////////////////////////////////////
// MODE menu
//////////////////////////////////////

void xaplMainWindow::OnMenuTogglePlayMode( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlayMode();
}

void xaplMainWindow::OnMenuModeRptList( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlayMode( PLAYMODE_LOOP_LIST );
}

void xaplMainWindow::OnMenuModeRptSong( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlayMode( PLAYMODE_LOOP_SONG );
}

void xaplMainWindow::OnMenuModeShuffle( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlayMode( PLAYMODE_SHUFFLE );
}

//////////////////////////////////////
// HELP menu
//////////////////////////////////////

void xaplMainWindow::OnMenuAbout( wxCommandEvent &WXUNUSED( evt ) )
{
    aboutDialog->Show();
}
