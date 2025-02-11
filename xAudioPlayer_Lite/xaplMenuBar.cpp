#include "xaplMenuBar.h"

xaplMenuBar::xaplMenuBar( void ) : wxMenuBar(), currentPlayMode( 0 ), currentPlaylistFormat( 0 )
{ 
	Initialize();
}

void xaplMenuBar::Initialize( void )
{
	menuFile = new wxMenu;
	menuView = new wxMenu;
	menuPlay = new wxMenu;
	menuMode = new wxMenu;
	menuHelp = new wxMenu;

	menuFile->Append( MenuItem_File_OpenFile, wxT("Add File(s)...\tF2") );
	menuFile->Append( MenuItem_File_OpenLocation, wxT("Add Location...\tF3") );
	menuFile->Append( MenuItem_File_SavePlaylist, wxT("Save Playlist...\tCtrl-S") );
	menuFile->Append( MenuItem_File_OpenPlaylist, wxT("Open Playlist...\tCtrl-O") );
	menuFile->AppendSeparator();
	menuFile->Append( MenuItem_File_Settings, wxT("Settings...\tF6") );
	menuFile->AppendSeparator();
	menuFile->Append( MenuItem_File_Exit, wxT("Exit") );
	
	menuView->AppendCheckItem( MenuItem_View_TogglePlaylist, wxT("Show Playlist\tCtrl-P") );
	menuView->AppendSeparator();
	menuView->Append( MenuItem_View_TogglePLFormat, wxT("Toggle Playlist View\tF4") );
	#if defined(__WINDOWS__) || defined(__WXGTK__)
	  menuView->AppendRadioItem( MenuItem_View_PLFileName, wxT("View as File Name") );
	  menuView->AppendRadioItem( MenuItem_View_PLFullPath, wxT("View as Full Path") );
	  menuView->AppendRadioItem( MenuItem_View_PLCustomTags, wxT("Customize View...") );
	#else
	  menuView->AppendCheckItem( MenuItem_View_PLFileName, wxT("View as File Name") );
	  menuView->AppendCheckItem( MenuItem_View_PLFullPath, wxT("View as Full Path") );
	  menuView->AppendCheckItem( MenuItem_View_PLCustomTags, wxT("Customize View...") );
	#endif

	menuPlay->AppendCheckItem( MenuItem_Play_SoftMute, wxT("Soft Mute\tHome") );
	menuPlay->Append( MenuItem_Play_VolumeUp, wxT("Volume Up\tUp") );
	menuPlay->Append( MenuItem_Play_VolumeDown, wxT("Volume Down\tDown") );
	menuPlay->AppendSeparator();
	menuPlay->Append( MenuItem_Play_Play, wxT("Play\tEnter") );
	menuPlay->Append( MenuItem_Play_Pause, wxT("Pause\tSpace") );
	menuPlay->Append( MenuItem_Play_Stop, wxT("Stop\t0") );
	menuPlay->Append( MenuItem_Play_Next, wxT("Next Song\tTab") );
	menuPlay->Append( MenuItem_Play_Prev, wxT("Previous Song\tShift-Tab") );

	menuMode->Append( MenuItem_Mode_Toggle, wxT("Toggle Play Mode\tF5") );
	#if defined(__WINDOWS__) || defined(__WXGTK__)
	  menuMode->AppendRadioItem( MenuItem_Mode_LoopList, wxT("Repeat Playlist") );
	  menuMode->AppendRadioItem( MenuItem_Mode_LoopSong, wxT("Repeat Song") );
	  menuMode->AppendRadioItem( MenuItem_Mode_Shuffle, wxT("Shuffle") );
	#else
	  menuMode->AppendCheckItem( MenuItem_Mode_LoopList, wxT("Repeat Playlist") );
	  menuMode->AppendCheckItem( MenuItem_Mode_LoopSong, wxT("Repeat Song") );
	  menuMode->AppendCheckItem( MenuItem_Mode_Shuffle, wxT("Shuffle") );
	#endif

	menuHelp->Append( MenuItem_Help_Contents, wxT("xAudio Player Lite Help...\tF1") );
	menuHelp->Append( MenuItem_Help_About, wxT("About...\tCtrl-A") );

	// Add to menus
	Append( menuFile, wxT("&File") );
	Append( menuView, wxT("&View") );
	Append( menuPlay, wxT("&Play") );
	Append( menuMode, wxT("&Mode") );
	Append( menuHelp, wxT("&Help") );

	// Default values
	menuView->Check( MenuItem_View_TogglePlaylist, true );
	menuView->Check( MenuItem_View_PLFileName, true );
	menuMode->Check( MenuItem_Mode_LoopList, true );
}

/* ====================================================================
 * ==== GETTERS =====================================================*/

bool xaplMenuBar::GetPlaylistVisible( void ) const
{
	return menuView->IsChecked( MenuItem_View_TogglePlaylist );
}

bool xaplMenuBar::GetSoftMuted( void ) const
{
	return menuPlay->IsChecked( MenuItem_Play_SoftMute );
}

int xaplMenuBar::GetPlaylistFormat( void ) const
{
	return currentPlaylistFormat;
}

int xaplMenuBar::GetPlayMode( void ) const
{
	return currentPlayMode;
}

/* ====================================================================
 * ==== SETTERS =====================================================*/

void xaplMenuBar::TogglePlaylistVisible( bool chk )
{
	menuView->Check( MenuItem_View_TogglePlaylist, chk );
}

void xaplMenuBar::ToggleSoftMute( bool chk )
{
	menuPlay->Check( MenuItem_Play_SoftMute, chk );
}

void xaplMenuBar::TogglePlaylistFormat( int format )
{
	// -1 == auto toggle...
	if ( format == -1 )
	{
		switch( currentPlaylistFormat )
		{
		case PLFORMAT_FILENAME:
			currentPlaylistFormat = PLFORMAT_FULLPATH;
			break;
		case PLFORMAT_FULLPATH:
			currentPlaylistFormat = PLFORMAT_CUSTOM;
			break;
		case PLFORMAT_CUSTOM:
			currentPlaylistFormat = PLFORMAT_FILENAME;
			break;
		default:
			currentPlaylistFormat = PLFORMAT_FILENAME;
			break;
		}

		format = currentPlaylistFormat;
	}
	else
	{
		if ( format == PLFORMAT_FILENAME || format == PLFORMAT_FULLPATH || format == PLFORMAT_CUSTOM )
			currentPlaylistFormat = format;
		else
			currentPlaylistFormat = PLFORMAT_FILENAME;
	}
	
	// This is a little messy, but it provides RadioItem-like
	// functionality for platforms that only support CheckItems
	switch ( format )
	{
	case PLFORMAT_FILENAME:
		menuView->Check( MenuItem_View_PLFileName, true );
		menuView->Check( MenuItem_View_PLFullPath, false );
		menuView->Check( MenuItem_View_PLCustomTags, false );
		break;

	case PLFORMAT_FULLPATH:
		menuView->Check( MenuItem_View_PLFileName, false );
		menuView->Check( MenuItem_View_PLFullPath, true );
		menuView->Check( MenuItem_View_PLCustomTags, false );
		break;

	case PLFORMAT_CUSTOM:
		menuView->Check( MenuItem_View_PLFileName, false );
		menuView->Check( MenuItem_View_PLFullPath, false );
		menuView->Check( MenuItem_View_PLCustomTags, true );
		break;

	default:
		break;
	}
}

void xaplMenuBar::TogglePlayMode( int mode )
{
	// -1 == auto toggle...
	if ( mode == -1 )
	{
		switch( currentPlayMode )
		{
		case PLAYMODE_SHUFFLE:
			currentPlayMode = PLAYMODE_LOOP_LIST;
			break;
		case PLAYMODE_LOOP_SONG:
			currentPlayMode = PLAYMODE_SHUFFLE;
			break;
		case PLAYMODE_LOOP_LIST:
			currentPlayMode = PLAYMODE_LOOP_SONG;
			break;
		default:
			currentPlayMode = PLAYMODE_LOOP_LIST;
			break;
		}

		mode = currentPlayMode;
	}
	else
	{
		if ( mode == PLAYMODE_SHUFFLE || mode == PLAYMODE_LOOP_SONG || mode == PLAYMODE_LOOP_LIST )
			currentPlayMode = mode;
		else 
			currentPlayMode = PLAYMODE_LOOP_LIST;
	}
	
	// This is a little messy, but it provides RadioItem-like
	// functionality for platforms that only support CheckItems
	switch ( mode )
	{
	case PLAYMODE_SHUFFLE:
		menuMode->Check( MenuItem_Mode_LoopList, false );
		menuMode->Check( MenuItem_Mode_LoopSong, false );
		menuMode->Check( MenuItem_Mode_Shuffle, true );
		break;

	case PLAYMODE_LOOP_SONG:
		menuMode->Check( MenuItem_Mode_LoopList, false );
		menuMode->Check( MenuItem_Mode_LoopSong, true );
		menuMode->Check( MenuItem_Mode_Shuffle, false );
		break;

	case PLAYMODE_LOOP_LIST:
		menuMode->Check( MenuItem_Mode_LoopList, true );
		menuMode->Check( MenuItem_Mode_LoopSong, false );
		menuMode->Check( MenuItem_Mode_Shuffle, false );
		break;

	default:
		break;
	}
}
