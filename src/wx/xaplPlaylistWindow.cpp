#include "xaplAudio.h"
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

/* ====================================================================
 * ==== EVENT TABLE DECLARATION =====================================*/

BEGIN_EVENT_TABLE( xaplPlaylistWindow, wxFrame )
	// Main menu items
	EVT_MENU( MenuItem_File_OpenFile, xaplPlaylistWindow::OnMenuOpenFile )
	EVT_MENU( MenuItem_File_OpenLocation, xaplPlaylistWindow::OnMenuOpenLocation )
	EVT_MENU( MenuItem_File_AddInternetStream, xaplPlaylistWindow::OnMenuAddInternetStream )
	EVT_MENU( MenuItem_File_ClearPlaylist, xaplPlaylistWindow::OnMenuClearPlaylist )
	EVT_MENU( MenuItem_File_NewPlaylist, xaplPlaylistWindow::OnMenuNewPlaylist )
	EVT_MENU( MenuItem_File_SavePlaylist, xaplPlaylistWindow::OnMenuSavePlaylist )
	EVT_MENU( MenuItem_File_OpenPlaylist, xaplPlaylistWindow::OnMenuOpenPlaylist )
	EVT_MENU( MenuItem_File_Exit, xaplPlaylistWindow::OnMenuExit )

	EVT_MENU( MenuItem_View_TogglePlaylist, xaplPlaylistWindow::OnMenuTogglePlaylist )
	EVT_MENU( MenuItem_View_TogglePLFormat, xaplPlaylistWindow::OnMenuTogglePlaylistFormat )
	EVT_MENU( MenuItem_View_PLFileName, xaplPlaylistWindow::OnMenuViewFilename )
	EVT_MENU( MenuItem_View_PLFullPath, xaplPlaylistWindow::OnMenuViewFullPath )
	EVT_MENU( MenuItem_View_PLCustomTags, xaplPlaylistWindow::OnMenuViewM3U )

	EVT_MENU( MenuItem_Play_SoftMute, xaplPlaylistWindow::OnMenuSoftMute )
	EVT_MENU( MenuItem_Play_VolumeUp, xaplPlaylistWindow::OnMenuVolumeUp)
	EVT_MENU( MenuItem_Play_VolumeDown, xaplPlaylistWindow::OnMenuVolumeDown )
	EVT_MENU( MenuItem_Play_Play, xaplPlaylistWindow::OnMenuPlay )
	EVT_MENU( MenuItem_Play_Pause, xaplPlaylistWindow::OnMenuPause )
	EVT_MENU( MenuItem_Play_Stop, xaplPlaylistWindow::OnMenuStop )
	EVT_MENU( MenuItem_Play_Next, xaplPlaylistWindow::OnMenuNextSong )
	EVT_MENU( MenuItem_Play_Prev, xaplPlaylistWindow::OnMenuPrevSong )

	EVT_MENU( MenuItem_Favorites_Add, xaplPlaylistWindow::OnMenuAddFavorite )
	EVT_MENU( MenuItem_Favorites_Manage, xaplPlaylistWindow::OnMenuManageFavorites )
	EVT_MENU_RANGE( MenuItem_Favorites_ListStart, MenuItem_Favorites_ListEnd, xaplPlaylistWindow::OnMenuLoadFavorite )

	EVT_MENU( MenuItem_Mode_Toggle, xaplPlaylistWindow::OnMenuTogglePlayMode )
	EVT_MENU( MenuItem_Mode_LoopList, xaplPlaylistWindow::OnMenuModeRptList )
	EVT_MENU( MenuItem_Mode_LoopSong, xaplPlaylistWindow::OnMenuModeRptSong )
	EVT_MENU( MenuItem_Mode_Shuffle, xaplPlaylistWindow::OnMenuModeShuffle )

	EVT_MENU( MenuItem_Tools_Settings, xaplPlaylistWindow::OnMenuSettings )
	EVT_MENU( MenuItem_Tools_ScrollSel, xaplPlaylistWindow::OnMenuScrollToSelection )
	EVT_MENU( MenuItem_Tools_ScrollPlaying, xaplPlaylistWindow::OnMenuScrollToPlaying )
	EVT_MENU( MenuItem_Tools_SleepTimer, xaplPlaylistWindow::OnMenuSleepTimer )
	EVT_MENU( MenuItem_Tools_ResetInvalidFlags, xaplPlaylistWindow::OnMenuResetInvalids )
	EVT_MENU( MenuItem_Tools_DelMissing, xaplPlaylistWindow::OnMenuDeleteMissing )
	EVT_MENU( MenuItem_Tools_DelDuplicate, xaplPlaylistWindow::OnMenuDeleteDuplicates )
	EVT_MENU( MenuItem_Tools_ViewHistory, xaplPlaylistWindow::OnMenuViewHistory )
	EVT_MENU( MenuItem_Tools_ClearHistory, xaplPlaylistWindow::OnMenuClearHistory )

	EVT_MENU( MenuItem_Help_Contents, xaplPlaylistWindow::OnMenuHelp )
	EVT_MENU( MenuItem_Help_CheckUpdate, xaplPlaylistWindow::OnMenuCheckUpdate )
	EVT_MENU( MenuItem_Help_ReportBug, xaplPlaylistWindow::OnMenuReportBug )
	EVT_MENU( MenuItem_Help_About, xaplPlaylistWindow::OnMenuAbout )

	// Context menu items
	EVT_MENU( ContextMenuItem_Queue, xaplPlaylistWindow::OnQueue )
	EVT_MENU( ContextMenuItem_Remove, xaplPlaylistWindow::OnRemove )
	EVT_MENU_RANGE( MenuItem_Favorites_ListStart + 100, MenuItem_Favorites_ListStart + 200, xaplPlaylistWindow::OnAddToFavorite )
	EVT_MENU( ContextMenuItem_NewFavorite, xaplPlaylistWindow::OnAddToNewFavorite )
	EVT_MENU( ContextMenuItem_Explore, xaplPlaylistWindow::OnExplore )	EVT_MENU( ContextMenuItem_Properties, xaplPlaylistWindow::OnProperties )

#ifdef __WINDOWS__
	// Hotkeys
	EVT_HOTKEY( -1, xaplPlaylistWindow::OnHotKey )
#endif

	// Playlist
	EVT_LISTBOX_DCLICK( ListBox_Playlist, xaplPlaylistWindow::OnPlaylistItemActivated )

	// Global key down handler
	EVT_KEY_DOWN( xaplPlaylistWindow::OnKeyDown )

	// Control buttons
	EVT_BUTTON( ID_PlayPauseButton, xaplPlaylistWindow::OnButtonPlayPause )
	EVT_BUTTON( ID_StopButton, xaplPlaylistWindow::OnButtonStop )
	EVT_BUTTON( ID_PrevSongButton, xaplPlaylistWindow::OnButtonPrevSong )
	EVT_BUTTON( ID_NextSongButton, xaplPlaylistWindow::OnButtonNextSong )

	// Window events
	EVT_SIZE( xaplPlaylistWindow::OnResize )
	EVT_MOVE( xaplPlaylistWindow::OnMove )
	EVT_CLOSE( xaplPlaylistWindow::OnClose )
	EVT_ICONIZE( xaplPlaylistWindow::OnIconize )
	EVT_MAXIMIZE( xaplPlaylistWindow::OnMaximize )
END_EVENT_TABLE()

/* ====================================================================
 * ==== (CON|DE)STRUCTORS AND INITIALIZATION ========================*/
xaplPlaylistWindow::xaplPlaylistWindow( void ) :
	wxFrame( (wxFrame *)NULL, wxID_ANY, wxT("Xapl"), wxPoint(1, 1), wxSize(335, 440), wxDEFAULT_FRAME_STYLE )
{	
	Initialize();
	ApplyConfig();

	// Set up and start up the position timer
	positionTimer = new xaplPlaybackTimer;
	positionTimer->AddTimeObserver( this );
	positionTimer->Create();
	positionTimer->SetPriority( 75 );
	positionTimer->Run();
}

xaplPlaylistWindow::~xaplPlaylistWindow( void )
{
	SaveConfig();
	playlist->SavePlaylist(); 

	positionTimer->Delete();

	taskBarIcon->SetIconVisible( false );
	delete taskBarIcon;
	
	delete favoritesMgr;
	delete history;
	delete sleepTimer;
	delete listContextMenu;
	delete controls;
	delete playlist;
}

void xaplPlaylistWindow::SetTempStatusText( const wxString &text )
{
	statusBar->PushStatusText( text, 1 );
}

void xaplPlaylistWindow::RestoreTempStatusText( void )
{
	statusBar->PopStatusText( 1 );
}

void xaplPlaylistWindow::UpdateSongCountStatusText( void )
{
	wxString status;
	status << playlist->GetItemCount() << ( playlist->GetItemCount() == 1 ? wxT("item") : wxT(" items") );
	statusBar->SetStatusText( status, 0 );
}

void xaplPlaylistWindow::OnPlayModeChanged( int playMode )
{
	menuBar->TogglePlayMode( playMode );

	// Set status bar text
	wxString modeStr;
	switch ( playMode )
	{
	case PLAYMODE_SHUFFLE:
		modeStr = wxT( "Shuffle" );
		break;
	case PLAYMODE_LOOP_SONG:
		modeStr = wxT( "Repeat Song" );
		break;
	case PLAYMODE_LOOP_LIST:
	default:
		modeStr = wxT( "Repeat List" );
		break;
	}
	playlist->SetPlayMode( playMode );
	statusBar->SetStatusText( modeStr, 1 );

	// No queueing unless playmode is loop list
	if ( playMode == PLAYMODE_LOOP_LIST )
		listContextMenu->Enable( ContextMenuItem_Queue, true );
	else
		listContextMenu->Enable( ContextMenuItem_Queue, false );
}

void xaplPlaylistWindow::Initialize( void )
{
	// Set the frame icon from the resource file	
	#ifdef __WINDOWS__
	  wxIcon appIco = wxIcon( wxICON( APPICON ) );
	#else
	  wxIcon appIco = wxIcon( app_xpm );
	#endif
	
	if ( appIco.Ok() )
		SetIcon( appIco );

	// Set up the system tray icon
	taskBarIcon = new xaplTaskBarIcon( this );
	
	// Set up the menu
	menuBar = new xaplMenuBar;
	SetMenuBar( menuBar );

	// Set up the status bar
	int widths[] = { -2, -2, -4 };
	statusBar = new wxStatusBar( this, wxID_ANY, wxST_SIZEGRIP );
	statusBar->SetFieldsCount( WXSIZEOF(widths) );
	statusBar->SetStatusWidths( WXSIZEOF(widths), widths );

	SetStatusBar( statusBar );
	SetStatusBarPane( -1 );

	// Playlist context menu
	addToFavorite = new wxMenu( wxEmptyString );
	addToFavorite->Append( ContextMenuItem_NewFavorite, wxT("New Favorite...") );

	listContextMenu = new wxMenu( wxEmptyString );
	listContextMenu->Append( ContextMenuItem_Queue, wxT("Queue\tIns"), wxT("") );
	listContextMenu->Append( ContextMenuItem_Remove, wxT("Remove\tDel"), wxT("") );
	listContextMenu->AppendSubMenu( addToFavorite, wxT("Add To") );
	listContextMenu->Append( ContextMenuItem_Explore, wxT("Explore...\tCtrl-E"), wxT("") );
	listContextMenu->AppendSeparator();
	listContextMenu->Append( ContextMenuItem_Properties, wxT("Properties...\tF2"), wxT("") );

	// Lay out the controls
	controls = new xaplControlUI( this );

	playlist = new xaplPlaylistCtrl( this, ListBox_Playlist );
	playlist->SetFocus();
	playlist->Connect( ListBox_Playlist, wxEVT_RIGHT_UP, wxMouseEventHandler(xaplPlaylistWindow::OnPlaylistRightClick), NULL, this );
	SetDropTarget( new xaplFileDropTarget( playlist ) );

	search = new xaplSearchUI( this, menuBar, playlist );

	wxFlexGridSizer *sizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	sizer->Add( controls, 0, wxALIGN_TOP | wxEXPAND );
	sizer->Add( playlist, 0, wxALIGN_TOP | wxEXPAND );
	sizer->Add( search, 0, wxALIGN_TOP | wxEXPAND );

	sizer->AddGrowableCol( 0 );
	sizer->AddGrowableRow( 1 );

	SetSizer( sizer );

	// Set playlist for xaplAudio
	xaplAudio::GetInstance()->SetPlaylist( this->playlist );

	// Set us up as an xaplAudioEventListener
	xaplAudio::GetInstance()->AddAudioEventListener( this );

	// Misc. setup
	favoritesMgr = new xaplFavoritesManager( playlist, menuBar, addToFavorite );
	history = new xaplHistoryDialog( this, playlist );

	prevVolume = 100;
	
	sleepTimer = new xaplSleepTimerUI( this, wxID_ANY );
	
#ifdef __WINDOWS__
	// System-wide hotkeys
	hotKeyHack = 0;
	bool a = this->RegisterHotKey( 0xBAFF, 0, KEY_PLAY_PAUSE );
	bool b = this->RegisterHotKey( 0xBBFF, 0, KEY_STOP );
	bool c = this->RegisterHotKey( 0xBCFF, 0, KEY_NEXT );
	bool d = this->RegisterHotKey( 0xBDFF, 0, KEY_PREV );
#endif
}

void xaplPlaylistWindow::ApplyConfig( void )
{
	xaplConfiguration *cfg = wxGetApp().GetConfig();
	loadingSettings = true;

	// Playlist visibility
	menuBar->TogglePlaylistVisible( cfg->plVisible );
	TogglePlaylist( cfg->plVisible );

	// Window minimum size
	if ( cfg->plVisible ) SetMinSize( wxSize(340, 400) );
	else SetMinSize( wxSize(340, 96) );

	// Always on top
	if ( cfg->plVisible )
		cfg->alwaysOnTop ? SetWindowStyle( wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE );
	else
		cfg->alwaysOnTop ? SetWindowStyle( (wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) );

	// Window size
	lastSize = cfg->windowSize;
	SetSize( cfg->windowSize );

	// Window position
	SetPosition( cfg->windowPosition );

	// Window transparency
#ifdef __WINDOWS__
	SetTransparent( (wxByte) cfg->windowTransparency );
#endif

	// Window maximized
	if ( cfg->windowMaximized ) Maximize( true );
	else maximized = false;

	// Window minimized
	if ( cfg->windowMinimized )
	{
		if ( cfg->minToTray )
		{
			Hide();
			taskBarIcon->SetIconVisible( true );
		}
		else
		{
			Iconize();
		}
	}

	// Playlist format
	menuBar->TogglePlaylistFormat( cfg->plFormat );
	playlist->SetFormat( cfg->plFormat );

	// Loaded playlist
	if ( !playlist->LoadPlaylist( cfg->loadedPlaylist ) )
		playlist->LoadPlaylist( wxString::Format( wxT("%s%s"), CONFIG_DIR.c_str(), wxT("Default.m3u") ) );
	UpdateSongCountStatusText();
	favoritesMgr->RefreshFavorites();

	// Playback mode
	OnPlayModeChanged( cfg->playMode );

	// Softmute mode
	menuBar->ToggleSoftMute( cfg->softMute );

	// Volume
	controls->SetVolume( cfg->volume );

	// Selected song
	if ( cfg->selectedFile >= 0 && cfg->selectedFile < (long) playlist->GetItemCount() )
	{
		playlist->DeselectAll();
		playlist->SetSelection( playlist->GetIndexByItemId( cfg->selectedFile ) );
		playlist->ScrollToLine( cfg->selectedFile );
	}

	// Search visible
	if ( cfg->searchVisible )
		search->Show();
	else
		search->Hide();

	// Search terms
	search->AppendToSearchBox( cfg->searchTerms );

	// Playlist style
	playlist->SetStyle( cfg->plStyle );

	loadingSettings = false;
}

void xaplPlaylistWindow::SaveConfig( void )
{
	xaplConfiguration *cfg = wxGetApp().GetConfig();
	
	cfg->plVisible = menuBar->GetPlaylistVisible();
	cfg->plFormat = menuBar->GetPlaylistFormat();
	cfg->loadedPlaylist = playlist->GetPlaylistFilePath();
	cfg->playMode = menuBar->GetPlayMode();
	cfg->softMute = menuBar->GetSoftMuted();
	cfg->volume = controls->GetVolume();
	cfg->selectedFile = playlist->GetPlayingItem();
	cfg->searchVisible = search->IsShown();
	cfg->searchTerms = search->GetSearchTerms();
}

void xaplPlaylistWindow::TogglePlaylist( bool showPL )
{
	if ( showPL )
	{
		statusBar->Show();
		playlist->Show();
		SetMinSize( wxSize(340, 400) );
		SetSize( lastSize );
		wxGetApp().GetConfig()->alwaysOnTop ? SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE );
	}
	else
	{
		lastSize = GetSize();
		statusBar->Hide();
		playlist->Hide();
		SetMinSize( wxSize(340, 96) );
		SetSize( 340, 96 );
		wxGetApp().GetConfig()->alwaysOnTop ? SetWindowStyle( (wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) );
	}

	wxGetApp().GetConfig()->plVisible = showPL;
	Layout();
	Refresh();
	Update();
}

/* ====================================================================
 * ==== NON-WX EVENTS ===============================================*/
void xaplPlaylistWindow::OnTrackChanged( xaplPlaylistItem *oldTrack, xaplPlaylistItem *newTrack )
{
	xaplAudio* audio = xaplAudio::GetInstance();
	if ( audio == NULL ) return;

	if ( newTrack != NULL )
	{
		playlist->SetPlayingItem( newTrack->id );
		statusBar->SetStatusText( audio->GetTrackInfo(), 2 );
		SetTitle( wxString::Format( wxT("%s - Xapl"), newTrack->fileName.c_str() ) );
		taskBarIcon->SetTooltip( newTrack->path );
		history->Append( newTrack->id );
	}

	controls->SetSongLength( audio->GetTrackLength() );
	controls->SetPlaying();
}

void xaplPlaylistWindow::OnPlayingStatusChanged( int prevStatus, int status, xaplPlaylistItem *currentTrack )
{
	if ( prevStatus == status ) 
		return;

	switch ( status )
	{
	case xaplPS_STOPPED:
		controls->SetSongLength( 0 );
		controls->SetPaused();

		playlist->SetPlayingItem( xaplNO_ITEM );

		taskBarIcon->SetTooltip( wxEmptyString );
		SetTitle( wxT("Xapl") );
		statusBar->SetStatusText( wxEmptyString, 2 );
		break;
	case xaplPS_PAUSED:
		controls->SetPaused();
		break;
	case xaplPS_PLAYING:
		controls->SetPlaying();
		break;
	}
}

void xaplPlaylistWindow::OnVolumeChanged( unsigned char oldVolume, unsigned char newVolume )
{
	prevVolume = oldVolume;

	if ( newVolume == 30 )
		menuBar->Check( MenuItem_Play_SoftMute, true );
	else
		menuBar->Check( MenuItem_Play_SoftMute, false );
}

void xaplPlaylistWindow::OnPositionChanged( float oldPosition, float newPosition )
{}

void xaplPlaylistWindow::OnTimeUpdated( void )
{
	xaplAudio* audio = xaplAudio::GetInstance();
	if ( audio != NULL && audio->GetPlaybackState() == xaplPS_PLAYING )
	{
		wxMutexGuiEnter();
		controls->SetSongPosition( audio->GetTrackPosition(), audio->GetTrackLength() );
		wxMutexGuiLeave();
	}
}

/* ====================================================================
 * ==== WX EVENTS ===================================================*/

//////////////////////////////////////
// Misc. events
//////////////////////////////////////

#ifdef __WINDOWS__
void xaplPlaylistWindow::OnHotKey( wxKeyEvent &evt )
{
	// Global media keys (probably Windows only)
	// About 'hotKeyHack': for some reason this hotkey is fired twice
	// in immediate succession, therefore having no effect. So this
	// ugly thing works around that. Also make sure it will never overflow,
	// for people who never close the app.
	if ( hotKeyHack + 1 == LONG_MAX )
		hotKeyHack = hotKeyHack % 2 == 0 ? 0 : 1;

	hotKeyHack++;
	
	wxUint32 rawFlags = evt.GetRawKeyFlags();
	if ( hotKeyHack % 2 == 0 )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio == NULL ) return;

		if ( rawFlags == 0xBAFF )
		{
			audio->Pause();
			OnButtonPlayPause( dummyCommand );
		}
		else if ( rawFlags == 0xBBFF )
		{
			audio->Stop();
			OnButtonStop( dummyCommand );
		}
		else if ( rawFlags == 0xBCFF )
		{
			audio->Next();
			OnButtonNextSong( dummyCommand );
		}
		else if ( rawFlags == 0xBDFF )
		{
			audio->Previous();	
			OnButtonPrevSong( dummyCommand );
		}
	}
}
#endif

void xaplPlaylistWindow::OnKeyDown( wxKeyEvent &evt )
{
	// Only process if the main window is active
	if ( !IsActive() ) return;
	
	int keyCode = evt.GetKeyCode();

	// Playlist search
	if ( playlist->IsShown() && !evt.ControlDown() && !evt.AltDown() && (
			(evt.GetKeyCode() >= 47 && evt.GetKeyCode() <= 57) || 
			(evt.GetKeyCode() >= 65 && evt.GetKeyCode() <= 90) || 
			(evt.GetKeyCode() >= 97 && evt.GetKeyCode() <= 122)
		))
	{

		if ( !search->IsShown() )
		{			
			playlist->DeselectAll();
			search->Show();
			search->FocusSearchBox();
			
			if ( evt.ShiftDown() )
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()) );
			else
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()).Lower() );
		}
		else if ( wxWindow::FindFocus() != search->GetSearchTextCtrl() )
		{
			search->FocusSearchBox();
			
			if ( evt.ShiftDown() )
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()) );
			else
				search->AppendToSearchBox( wxString(evt.GetUnicodeKey()).Lower() );
		}
	}
	else if ( keyCode == WXK_ESCAPE && search->IsShown() )
	{
		OnSearchBarHidden( false );
	}
	else if ( keyCode == WXK_RETURN && search->IsShown() )
	{
		OnSearchBarHidden( true );
	}
	// Left / Right keys for seeking
	else if ( evt.ShiftDown() && keyCode == WXK_LEFT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_BACKWARD, xaplSF_SMALL );
	}
	else if ( evt.ShiftDown() && keyCode == WXK_RIGHT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_FORWARD, xaplSF_SMALL );
	}
	else if ( evt.ControlDown() && keyCode == WXK_LEFT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_BACKWARD, xaplSF_LARGE );
	}
	else if ( evt.ControlDown() && keyCode == WXK_RIGHT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_FORWARD, xaplSF_LARGE );
	}
	else if ( keyCode == WXK_LEFT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_BACKWARD, xaplSF_MEDIUM );
	}
	else if ( keyCode == WXK_RIGHT )
	{
		xaplAudio* audio = xaplAudio::GetInstance();
		if ( audio->GetPlaybackState() == xaplPS_PLAYING || audio->GetPlaybackState() == xaplPS_PAUSED )
			audio->Seek( xaplSF_FORWARD, xaplSF_MEDIUM );
	}
	// Up / Down keys for playlist
	else if ( keyCode == WXK_UP )
	{
		int sel = (int)playlist->GetFirstSelection() - 1;
		playlist->DeselectAll();
		if ( sel < 0 ) sel = (int)playlist->GetItemCount() - 1;
		playlist->SetSelection( sel );
		return;
	}
	else if ( keyCode == WXK_DOWN )
	{
		int sel = (int)playlist->GetFirstSelection() + 1;
		playlist->DeselectAll();
		if ( sel > playlist->GetItemCount() - 1 ) sel = 0;
		playlist->SetSelection( sel );
		return;
	}
	// Playlist context menu accelerators
	else if ( evt.ShiftDown() && keyCode == WXK_F2 ) // Shift-F2
	{
		OnPlayingProperties( dummyCommand );
	}
	else if ( keyCode == WXK_F2 ) // F2
	{
		OnProperties( dummyCommand );
	}
	else if ( !evt.ControlDown() && keyCode == WXK_DELETE ) // Delete
	{
		playlist->DeleteSelected();
		UpdateSongCountStatusText();
		if ( search->IsShown() )
			search->FocusSearchBox();
	}
#ifdef __WINDOWS__
	else if ( evt.ControlDown() && keyCode == 69 ) // Ctrl-E
	{
		OnExplore( dummyCommand );
	}
#endif
	// Others
	else if ( evt.ControlDown() && evt.ShiftDown() && evt.AltDown() && keyCode == 74 ) // It's a secret. Shut up!
	{
		wxMessageBox( wxT("Congratulations, you found the secret message! Maybe someday we'll put something interesting here ;)"), wxT("Xapl Secret Message"));
	}
	// These aren't handled automatically because they use only modifier keys
	else if ( evt.ControlDown() && evt.ShiftDown() )
	{
		OnMenuScrollToSelection( dummyCommand );
	}
	else if ( evt.ControlDown() && evt.AltDown() )
	{
		OnMenuScrollToPlaying( dummyCommand );
	}
	
	evt.Skip();
}

void xaplPlaylistWindow::OnResize( wxSizeEvent &evt )
{
	if ( !loadingSettings ) 
	{
		if ( !IsMaximized() ) 
		{
			if ( maximized ) // Hack to get around lack of a Restore event
			{
				SetSize( wxGetApp().GetConfig()->windowSize );
				maximized = false;
			}
			else
			{
				wxGetApp().GetConfig()->windowSize = GetSize();
			}
		}
		wxGetApp().GetConfig()->windowMaximized = IsMaximized();
	}
	evt.Skip();
}

void xaplPlaylistWindow::OnMove( wxMoveEvent &evt )
{
	if ( !loadingSettings && !IsMaximized() && evt.GetPosition().x != -32000 && evt.GetPosition().y != -32000 ) 
		wxGetApp().GetConfig()->windowPosition = evt.GetPosition();
	
	evt.Skip();
}

void xaplPlaylistWindow::OnClose( wxCloseEvent &evt )
{
	Hide();
	if ( xaplAudio::GetInstance()->Stop( 1500 ) )
		wxMilliSleep( 1500 );
	evt.Skip();
}

void xaplPlaylistWindow::OnIconize( wxIconizeEvent &evt )
{
	if ( !loadingSettings ) wxGetApp().GetConfig()->windowMinimized = true;
	
	if ( wxGetApp().GetConfig()->minToTray )
	{
		taskBarIcon->SetIconVisible( true );
		Hide();
	}

	evt.Skip();
}

void xaplPlaylistWindow::OnMaximize( wxMaximizeEvent &evt )
{
	if ( !loadingSettings ) wxGetApp().GetConfig()->windowMaximized = IsMaximized();
	maximized = true;
	Layout();
	Update();
	evt.Skip();
}

//////////////////////////////////////
// Control events
//////////////////////////////////////

void xaplPlaylistWindow::OnPlaylistItemActivated( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( search->IsShown() )
	{
		OnSearchBarHidden( true );
	}
	else
	{
		playlist->SetPlayingItem( playlist->GetFirstSelectedItemId() );
		xaplAudio::GetInstance()->Play();
	}
}

void xaplPlaylistWindow::OnButtonPlayPause( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetItemCount() == 0 || playlist->GetSelectedCount() == 0 ) 
		return;
	
	xaplAudio* audio = xaplAudio::GetInstance();
	if ( audio->GetPlaybackState() != xaplPS_PLAYING && audio->GetPlaybackState() != xaplPS_PAUSED )
	{
		playlist->SetPlayingItem( playlist->GetFirstSelectedItemId() );
		controls->TogglePlayPauseIcon();
		audio->Play();
	}
	else
	{
		OnMenuPause( dummyCommand );
	}
}

void xaplPlaylistWindow::OnButtonStop( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuStop( dummyCommand );
}

void xaplPlaylistWindow::OnButtonPrevSong( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuPrevSong( dummyCommand );
}

void xaplPlaylistWindow::OnButtonNextSong( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuNextSong( dummyCommand );
}

void xaplPlaylistWindow::OnPlaylistRightClick( wxMouseEvent &evt )
{
	if ( playlist->GetItemCount() == 0 ) return;

	if ( playlist->GetSelectedCount() <= 1 )
	{
		int index = playlist->HitTest( evt.GetPosition() );
		playlist->DeselectAll();
		playlist->Select( index );
	}

	playlist->PopupMenu( listContextMenu );
}

void xaplPlaylistWindow::OnSearchBarHidden( bool itemChosen )
{
	if ( itemChosen )
	{
		long id = playlist->GetFirstSelectedItemId();
		if ( id != xaplNO_ITEM )
		{
			playlist->SetPlayingItem( id );
			search->OnSearchBarHidden( dummyCommand );
			xaplAudio::GetInstance()->Play();
			controls->SetPlaying();
		}
		else
		{
			wxBell();
		}
	}
	else
	{
		search->OnSearchBarHidden( dummyCommand );
	}
}

//////////////////////////////////////
// FILE menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuOpenFile( wxCommandEvent &WXUNUSED( evt ) )
{
	wxFileDialog *fileDlg = new wxFileDialog( this,
											  wxT("Open Audio Files"),
											  wxEmptyString, 
											  wxEmptyString, 
											  xaplAudio::GetInstance()->GetCompletePlayableFileSpec(), 
											  wxOPEN | wxMULTIPLE | wxFILE_MUST_EXIST );
	if ( fileDlg->ShowModal() == wxID_OK )
	{
		SetTempStatusText( wxT("Opening...") );
		wxArrayString files;
		fileDlg->GetPaths( files );
		playlist->AddItems( files );
		UpdateSongCountStatusText();
		RestoreTempStatusText();
	}

	delete fileDlg;
}

void xaplPlaylistWindow::OnMenuOpenLocation( wxCommandEvent &WXUNUSED( evt ) )
{
	wxDirDialog *dirDlg = new wxDirDialog( this, wxT("Open a Directory of Audio Files"), wxGetApp().GetConfig()->lastOpenedDir, wxDD_DEFAULT_STYLE | ~wxDD_NEW_DIR_BUTTON );
	if ( dirDlg->ShowModal() == wxID_OK )
	{
		SetTempStatusText( wxT("Opening...") );
		wxGetApp().GetConfig()->lastOpenedDir = dirDlg->GetPath();
		playlist->AddItem( dirDlg->GetPath() );
		UpdateSongCountStatusText();
		RestoreTempStatusText();
	}

	delete dirDlg;
}

void xaplPlaylistWindow::OnMenuAddInternetStream( wxCommandEvent &WXUNUSED( evt ) )
{
	wxString url = wxGetTextFromUser( wxT("Enter the URL of the Internet stream:"), wxT("Xapl"), wxEmptyString, this );
}

void xaplPlaylistWindow::OnMenuClearPlaylist( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuStop( dummyCommand );
	playlist->Clear();
	history->Clear();
}

void xaplPlaylistWindow::OnMenuNewPlaylist( wxCommandEvent &WXUNUSED( evt ) )
{
	OnMenuStop( dummyCommand );

	wxString defaultPl = wxString::Format( wxT("%s%s"), CONFIG_DIR.c_str(), wxT("Default.m3u") );
	
	// Save current playlist if it is not the default
	if ( playlist->GetPlaylistFilePath().Lower() != defaultPl.Lower() )
	{
		SetTempStatusText( wxT("Saving...") );
		playlist->SavePlaylist();
		playlist->Clear();
		RestoreTempStatusText();
	}

	// Clear default playlist and load it
	wxFile file;
	file.Create( defaultPl, true );
	file.Write( wxT("#EXTM3U\n") );
	file.Close();

	playlist->LoadPlaylist( defaultPl );
	history->Clear();
	UpdateSongCountStatusText();
}

void xaplPlaylistWindow::OnMenuSavePlaylist( wxCommandEvent &WXUNUSED( evt ) )
{
	wxFileDialog *fileDlg = new wxFileDialog( this, wxT("Save Playlist As"), wxEmptyString, wxT("Playlist.m3u"), wxT("M3U Playlist (*.m3u)|*.m3u|PLS Playlist (*.pls)|*.pls|Text Playlist (*.txt)|*.txt"), wxSAVE | wxFD_CHANGE_DIR );
	if ( fileDlg->ShowModal() == wxID_OK )
	{
		SetTempStatusText( wxT("Saving...") );
		playlist->SavePlaylistAs( fileDlg->GetPath() );
		RestoreTempStatusText();
	}

	delete fileDlg;
}

void xaplPlaylistWindow::OnMenuOpenPlaylist( wxCommandEvent &WXUNUSED( evt ) )
{
	wxFileDialog *fileDlg = new wxFileDialog( this, wxT("Open Playlist"), wxEmptyString, wxEmptyString, wxT("Playlist Files (*.m3u, *.pls, *.txt)|*.m3u;*.pls;*.txt|M3U Playlist Files (*.m3u)|*.m3u|PLS Playlist Files (*.pls)|*.pls|Text Playlist Files (*.txt)|*.txt"), wxOPEN | wxFD_CHANGE_DIR );
	if ( fileDlg->ShowModal() == wxID_OK )
	{
		OnMenuStop( dummyCommand );

		SetTempStatusText( wxT("Saving...") );
		playlist->SavePlaylist();
		RestoreTempStatusText();
		SetTempStatusText( wxT("Opening...") );
		playlist->LoadPlaylist( fileDlg->GetPath() );
		favoritesMgr->RefreshFavorites();
		UpdateSongCountStatusText();
		RestoreTempStatusText();
	}

	delete fileDlg;
}

void xaplPlaylistWindow::OnMenuExit( wxCommandEvent &WXUNUSED( evt ) )
{
    Close( true );
}

//////////////////////////////////////
// VIEW menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuTogglePlaylist( wxCommandEvent &evt )
{
	TogglePlaylist( evt.IsChecked() );
}

void xaplPlaylistWindow::OnMenuTogglePlaylistFormat( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat();
	playlist->SetFormat( menuBar->GetPlaylistFormat() );
}

void xaplPlaylistWindow::OnMenuViewFilename( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_FILENAME );
	playlist->SetFormat( menuBar->GetPlaylistFormat() );
}

void xaplPlaylistWindow::OnMenuViewFullPath( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_FULLPATH );
	playlist->SetFormat( menuBar->GetPlaylistFormat() );
}

void xaplPlaylistWindow::OnMenuViewM3U( wxCommandEvent &WXUNUSED( evt ) )
{
	menuBar->TogglePlaylistFormat( PLFORMAT_EXTM3U );
	playlist->SetFormat( menuBar->GetPlaylistFormat() );
}

//////////////////////////////////////
// CONTROL menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuSoftMute( wxCommandEvent &evt )
{
	xaplAudio* audio = xaplAudio::GetInstance();
	if ( evt.IsChecked() )
	{
		unsigned char prevVol = audio->GetVolume();
		prevVolume = prevVol;

		// Don't raise the volume if it is already below soft mute level
		if ( prevVolume > 30 )
		{
			audio->SetVolume( 30 );
			controls->SetVolume( 30 );
		}
	}
	else
	{
		audio->SetVolume( prevVolume );
		controls->SetVolume( prevVolume );
	}
}

void xaplPlaylistWindow::OnMenuVolumeDown( wxCommandEvent &evt )
{
	int vol = controls->GetVolume() - 10;

	// Are we at the 'soft mute' volume?
	if ( vol == 30 )
		menuBar->ToggleSoftMute( true );
	else
		menuBar->ToggleSoftMute( false );

	xaplAudio::GetInstance()->SetVolume( vol );
	controls->SetVolume( vol );
}

void xaplPlaylistWindow::OnMenuVolumeUp( wxCommandEvent &evt )
{
	int vol = controls->GetVolume() + 10;

	// Are we at the 'soft mute' volume?
	if ( vol == 30 )
		menuBar->ToggleSoftMute( true );
	else
		menuBar->ToggleSoftMute( false );

	xaplAudio::GetInstance()->SetVolume( vol );
	controls->SetVolume( vol );
}

void xaplPlaylistWindow::OnMenuPlay( wxCommandEvent &evt )
{
	if ( playlist->GetSelectedCount() == 0 ) return;
	
	if ( xaplAudio::GetInstance()->GetPlaybackState() == xaplPS_PAUSED )
	{
		xaplAudio::GetInstance()->Play();
	}
	else
	{
		playlist->SetPlayingItem( playlist->GetFirstSelectedItemId() );
		xaplAudio::GetInstance()->Play();
	}

	controls->SetPlaying();
}

void xaplPlaylistWindow::OnMenuPause( wxCommandEvent &evt )
{
	// If paused or stopped, resume playback
	xaplAudio* audio = xaplAudio::GetInstance();
	int state = audio->GetPlaybackState();
	if ( state == xaplPS_PAUSED )
	{
		controls->TogglePlayPauseIcon();
		xaplAudio::GetInstance()->Play();
		return;
	}
	else if ( state == xaplPS_STOPPED )
	{
		if ( playlist->GetSelectedCount() == 0 ) return;

		playlist->SetPlayingItem( playlist->GetFirstSelectedItemId() );
		xaplAudio::GetInstance()->Play();
		return;
	}

	// Otherwise, pause playback
	controls->TogglePlayPauseIcon();
	xaplAudio::GetInstance()->Pause();
}

void xaplPlaylistWindow::OnMenuStop( wxCommandEvent &evt )
{
	controls->SetSongLength( 0 );
	controls->SetPaused();
	xaplAudio::GetInstance()->Stop();
	playlist->SetPlayingItem( xaplNO_ITEM );

	taskBarIcon->SetTooltip( wxEmptyString );
	SetTitle( wxT("Xapl") );
	statusBar->SetStatusText( wxEmptyString, 2 );
}

void xaplPlaylistWindow::OnMenuNextSong( wxCommandEvent &evt )
{
	if ( playlist->GetItemCount() == 0 ) return;

	playlist->Next();
	controls->SetPlaying();
	xaplAudio::GetInstance()->Play();
}

void xaplPlaylistWindow::OnMenuPrevSong( wxCommandEvent &evt )
{
	if ( playlist->GetItemCount() == 0 ) return;

	playlist->Previous();
	controls->SetPlaying();
	xaplAudio::GetInstance()->Play();
}

//////////////////////////////////////
// FAVORITES menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuAddFavorite( wxCommandEvent &WXUNUSED( evt ) )
{
	favoritesMgr->AddFavorite( playlist->GetPlaylistFilePath() );
}

void xaplPlaylistWindow::OnMenuManageFavorites( wxCommandEvent &WXUNUSED( evt ) )
{
	favoritesMgr->ManageFavorites();
}

void xaplPlaylistWindow::OnMenuLoadFavorite( wxCommandEvent &evt )
{
	wxString path = favoritesMgr->GetPath( evt.GetId() );
	if ( path.IsEmpty() )
	{
		wxMessageBox( wxT("The selected favorite playlist is invalid."), wxT("Xapl"), wxICON_EXCLAMATION | wxOK );
		return;
	}

	OnMenuStop( dummyCommand );

	SetTempStatusText( wxT("Saving...") );
	playlist->SavePlaylist();
	RestoreTempStatusText();
	SetTempStatusText( wxT("Opening...") );
	playlist->LoadPlaylist( path );
	favoritesMgr->RefreshFavorites();
	RestoreTempStatusText();

	UpdateSongCountStatusText();
}

//////////////////////////////////////
// MODE menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuTogglePlayMode( wxCommandEvent &WXUNUSED( evt ) )
{
	int mode = menuBar->TogglePlayMode();
	OnPlayModeChanged( mode );
}

void xaplPlaylistWindow::OnMenuModeRptList( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->EmptyQueue();
	OnPlayModeChanged( PLAYMODE_LOOP_LIST );
}

void xaplPlaylistWindow::OnMenuModeRptSong( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->EmptyQueue();
	OnPlayModeChanged( PLAYMODE_LOOP_SONG );
}

void xaplPlaylistWindow::OnMenuModeShuffle( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->EmptyQueue();
	OnPlayModeChanged( PLAYMODE_SHUFFLE );
}

//////////////////////////////////////
// TOOLS menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuSettings( wxCommandEvent &WXUNUSED( evt ) )
{
	xaplSettingsDialog *settings = new xaplSettingsDialog( this );
	if ( settings->ShowModal() == wxID_OK )
	{
		settings->Commit();
		xaplConfiguration *cfg = wxGetApp().GetConfig();

		// Apply changes
#ifdef __WINDOWS__
		SetTransparent( cfg->windowTransparency );
#endif
		playlist->SetStyle( cfg->plStyle );
		wxGetApp().MakeSingleInstance( cfg->singleInstance );

		if ( cfg->plVisible )
			cfg->alwaysOnTop ? SetWindowStyle( wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE );
		else
			cfg->alwaysOnTop ? SetWindowStyle( (wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) | wxSTAY_ON_TOP ) : SetWindowStyle( wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) );
		
		// Type associations
		if ( cfg->mp3 ) 
			xaplUtilities::SetTypeAssociation( wxT(".mp3"), wxT("audio/mpeg"), wxT("MP3 File") );
		else
			xaplUtilities::UnSetTypeAssociation( wxT(".mp3") );

		if ( cfg->ogg ) 
			xaplUtilities::SetTypeAssociation( wxT(".ogg"), wxT("audio/ogg"), wxT("OGG File") );
		else
			xaplUtilities::UnSetTypeAssociation( wxT(".ogg") );

		if ( cfg->wav ) 
			xaplUtilities::SetTypeAssociation( wxT(".wav"), wxT("audio/x-wav"), wxT("WAV File") );
		else
			xaplUtilities::UnSetTypeAssociation( wxT(".wav") );

		if ( cfg->wma ) 
			xaplUtilities::SetTypeAssociation( wxT(".wma"), wxT("audio/x-ms-wma"), wxT("WMA File") );
		else
			xaplUtilities::UnSetTypeAssociation( wxT(".wma") );

		if ( cfg->flac ) 
			xaplUtilities::SetTypeAssociation( wxT(".flac"), wxT("audio/flac"), wxT("FLAC File") );
		else
			xaplUtilities::UnSetTypeAssociation( wxT(".flac") );

		if ( cfg->aac ) 
		{
			xaplUtilities::SetTypeAssociation( wxT(".aac"), wxT("audio/mp4a-latm"), wxT("AAC File") );
			xaplUtilities::SetTypeAssociation( wxT(".m4a"), wxT("audio/mp4a-latm"), wxT("AAC File") );
		}
		else
		{
			xaplUtilities::UnSetTypeAssociation( wxT(".aac") );
			xaplUtilities::UnSetTypeAssociation( wxT(".m4a") );
		}
	}
	delete settings;
}

void xaplPlaylistWindow::OnMenuScrollToSelection( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->ScrollToSelection();
}

void xaplPlaylistWindow::OnMenuScrollToPlaying( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetPlayingItem() < 0 ) 
		playlist->ScrollToSelection(); // For convenience
	else
		playlist->ScrollToPlayingItem( true );
}

void xaplPlaylistWindow::OnMenuSleepTimer( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( !sleepTimer->IsShown() )
	{
		sleepTimer->Show();
	}
}

void xaplPlaylistWindow::OnMenuResetInvalids( wxCommandEvent &WXUNUSED( evt ) )
{
	SetTempStatusText( wxT("Please wait...") );
	playlist->ResetOkStatusForAllItems();
	playlist->Refresh();
	RestoreTempStatusText();
}

void xaplPlaylistWindow::OnMenuDeleteMissing( wxCommandEvent &WXUNUSED( evt ) )
{
	SetTempStatusText( wxT("Please wait...") );
	playlist->ResetOkStatusForAllItems();
	UpdateSongCountStatusText();
	RestoreTempStatusText();
}

void xaplPlaylistWindow::OnMenuDeleteDuplicates( wxCommandEvent &WXUNUSED( evt ) )
{
	SetTempStatusText( wxT("Please wait...") );
	playlist->RemoveDuplicateItems();
	UpdateSongCountStatusText();
	RestoreTempStatusText();
}

void xaplPlaylistWindow::OnMenuViewHistory( wxCommandEvent &WXUNUSED( evt ) )
{
	history->Show();
}

void xaplPlaylistWindow::OnMenuClearHistory( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->ClearHistory();
	history->Clear();
}

//////////////////////////////////////
// HELP menu
//////////////////////////////////////

void xaplPlaylistWindow::OnMenuHelp( wxCommandEvent &WXUNUSED( evt ) )
{
	wxMessageBox( wxT("Help documents are forthcoming."), wxT("Xapl") );
}

void xaplPlaylistWindow::OnMenuCheckUpdate( wxCommandEvent &WXUNUSED( evt ) )
{
	// No error dialogs
	wxLogNull logNull;

#ifdef __WINDOWS__
	wxString exeName = wxT("xAutoUpdater.exe");
#else
	wxString exeName = wxT("xAutoUpdater");
#endif

	// The extra space in between the 5th arg and the \" is required on Windows,
	// as the trailing backslash from the path escapes the backslash on the
	// double quote... thus messing up the rest of the arguments
	wxString cmd = wxT("%s %s %s %s \"%s \" %s %u");
	wxString expCmd = wxString::Format( cmd,
										wxString::Format( wxT("%s%s"), EXECUTABLE_DIR, exeName ),
										wxT("Xapl"),
										APP_VERSION,
										wxT("http://services.xivesoftware.com/updates/broadcast.xml"),
										EXECUTABLE_DIR,
										wxT("Xapl.exe"),
										2 );


	if ( wxExecute( expCmd ) == 0 )
		wxMessageBox( wxT("The update checker could not be started."), wxT("Xapl") );
}

void xaplPlaylistWindow::OnMenuReportBug( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( !wxLaunchDefaultBrowser( wxT("http://trac.xivesoftware.com/trac/xapl.cgi/newticket") ) )
		wxMessageBox( wxT("Your web browser could not be opened to submit a bug report. You can manually enter a bug report at http://trac.xivesoftware.com/trac/xapl.cgi/newticket."), wxT("Xapl") );
}

void xaplPlaylistWindow::OnMenuAbout( wxCommandEvent &WXUNUSED( evt ) )
{
    xaplAboutDialog *aboutDlg = new xaplAboutDialog( this );
	aboutDlg->ShowModal();
	delete aboutDlg;
}

//////////////////////////////////////
// Context Menu
//////////////////////////////////////

void xaplPlaylistWindow::OnQueue( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( menuBar->GetPlayMode() == PLAYMODE_LOOP_LIST )
	{
		wxArrayInt selections;
		playlist->GetSelectedIds( selections );
		playlist->QueueItems( selections );
	}
}

void xaplPlaylistWindow::OnRemove( wxCommandEvent &WXUNUSED( evt ) )
{
	playlist->DeleteSelected();
	UpdateSongCountStatusText();
}

void xaplPlaylistWindow::OnAddToFavorite( wxCommandEvent &evt )
{
	if ( playlist->GetSelectedCount() == 0 ) return;

	// Collect data
	int id = evt.GetId();
	std::string path = string( favoritesMgr->GetPath( id ).mb_str() );
	if ( path.empty() ) return;

	XplData **data = new XplData*[playlist->GetSelectedCount()];
	wxArrayInt selections;
	playlist->GetSelections( selections );

	// Assemble data
	for ( size_t i = 0; i < selections.GetCount(); ++i )
	{
		xaplPlaylistItem *plData = playlist->GetItemById( playlist->GetItemIdByIndex( selections.Item(i) ) );
		XplData *m3u = new XplData( string( plData->path.mb_str() ), 
								    string( plData->m3uLength.mb_str() ), 
								    string( plData->m3uTitle.mb_str() ) );
		data[i] = m3u;
	}

	// Perform append
	XplPlaylistHelper::AutoAppendData( data, selections.GetCount(), path );

	// Cleanup (done mostly by xPlaylistLib)
	delete [] data;
}

void xaplPlaylistWindow::OnAddToNewFavorite( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetSelectedCount() == 0 ) return;
	
	// Collect data
	xaplAddFavoriteUI *add = new xaplAddFavoriteUI( this );
	int result = add->ShowModal();
	if ( result != wxID_OK ) return;
	wxString wxPath = add->GetPlaylistPath();
	if ( wxPath == wxEmptyString ) return;

	std::string path = string( wxPath.mb_str() );

	XplData **data = new XplData*[playlist->GetSelectedCount()];
	wxArrayInt selections;
	playlist->GetSelections( selections );

	// Assemble data
	for ( size_t i = 0; i < selections.GetCount(); ++i )
	{
		xaplPlaylistItem *plData = playlist->GetItemById( playlist->GetItemIdByIndex( selections.Item(i) ) );
		XplData *m3u = new XplData( string( plData->path.mb_str() ), 
								    string( plData->m3uLength.mb_str() ), 
								    string( plData->m3uTitle.mb_str() ) );
		data[i] = m3u;
	}

	// Perform append
	XplPlaylistHelper::AutoAppendData( data, selections.GetCount(), path );
	favoritesMgr->RefreshFavorites();

	// Cleanup (done mostly by xPlaylistLib)
	delete [] data;
}

void xaplPlaylistWindow::OnExplore( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetSelectedCount() == 0 ) return;

#ifdef __WINDOWS__
	wxExecute( wxString::Format( EXPLORE_CMD, playlist->GetFirstSelectedFilePath().c_str() ) );
#else
	wxFileName fn( playlist->GetFirstSelectedFilePath() );
	wxExecute( wxString::Format( EXPLORE_CMD, fn.GetPath().c_str() ) );
#endif
}


void xaplPlaylistWindow::OnProperties( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetSelectedCount() == 0 ) return;
	
	xaplPropertiesDialog *props = new xaplPropertiesDialog( this, playlist->GetFirstSelectedPlaylistItem() );
	int result = props->ShowModal();

	if ( result == wxID_OK )
	{
		int changes = props->CommitChanges();
		
		if ( changes == 2 )
			playlist->ReloadFirstSelectedItem();
	}

	delete props;
}

void xaplPlaylistWindow::OnPlayingProperties( wxCommandEvent &WXUNUSED( evt ) )
{
	if ( playlist->GetSelectedCount() == 0 ) return;

	xaplPropertiesDialog *props = new xaplPropertiesDialog( this, playlist->GetPlayingItemData() );
	int result = props->ShowModal();

	if ( result == wxID_OK )
	{
		int changes = props->CommitChanges();
		
		if ( changes == 2 )
			playlist->ReloadFirstSelectedItem();
	}

	delete props;
}
