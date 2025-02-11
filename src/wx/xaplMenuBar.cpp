#include "xaplMenuBar.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

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
	menuFavorites = new wxMenu;
	menuTools = new wxMenu;
	menuHelp = new wxMenu;

	menuFile->Append( MenuItem_File_OpenFile, wxT("Add File(s)...\tF3") );
	menuFile->Append( MenuItem_File_OpenLocation, wxT("Add Location...\tF4") );
	menuFile->Append( MenuItem_File_AddInternetStream, wxT("Add Internet Stream...\tF5") );
	menuFile->AppendSeparator();
	menuFile->Append( MenuItem_File_ClearPlaylist, wxT("Clear Playlist\tCtrl-Alt-N") );
	menuFile->Append( MenuItem_File_NewPlaylist, wxT("New Playlist\tCtrl-N") );
	menuFile->Append( MenuItem_File_SavePlaylist, wxT("Save Playlist As...\tCtrl-S") );
	menuFile->Append( MenuItem_File_OpenPlaylist, wxT("Open Playlist...\tCtrl-O") );
	menuFile->AppendSeparator();
	menuFile->Append( MenuItem_File_Exit, wxT("Exit") );
	
	menuView->AppendCheckItem( MenuItem_View_TogglePlaylist, wxT("Show Playlist\tCtrl-P") );
	menuView->AppendSeparator();
	menuView->Append( MenuItem_View_TogglePLFormat, wxT("Toggle Playlist View\tF6") );
	#if defined(__WINDOWS__) || defined(__WXGTK__)
	  menuView->AppendRadioItem( MenuItem_View_PLFileName, wxT("View as File Name") );
	  menuView->AppendRadioItem( MenuItem_View_PLFullPath, wxT("View as Full Path") );
	  menuView->AppendRadioItem( MenuItem_View_PLCustomTags, wxT("View as Extended M3U Data") );
	#else
	  menuView->AppendCheckItem( MenuItem_View_PLFileName, wxT("View as File Name") );
	  menuView->AppendCheckItem( MenuItem_View_PLFullPath, wxT("View as Full Path") );
	  menuView->AppendCheckItem( MenuItem_View_PLCustomTags, wxT("View as Extended M3U Data") );
	#endif

	menuPlay->AppendCheckItem( MenuItem_Play_SoftMute, wxT("Soft Mute\tHome") );
	menuPlay->Append( MenuItem_Play_VolumeUp, wxT("Volume Up\tPgUp") );
	menuPlay->Append( MenuItem_Play_VolumeDown, wxT("Volume Down\tPgDn") );
	menuPlay->AppendSeparator();
	menuPlay->Append( MenuItem_Play_Play, wxT("Play\tEnter") );
	menuPlay->Append( MenuItem_Play_Pause, wxT("Pause\tSpace") );
	menuPlay->Append( MenuItem_Play_Stop, wxT("Stop\tEnd") );
	menuPlay->Append( MenuItem_Play_Next, wxT("Next Song\tTab") );
	menuPlay->Append( MenuItem_Play_Prev, wxT("Previous Song\tShift-Tab") );

	menuFavorites->Append( MenuItem_Favorites_Add, wxT("Add playlist to favorites...\tCtrl-D") );
	menuFavorites->Append( MenuItem_Favorites_Manage, wxT("Manage favorites...\tCtrl-B") );
	menuFavorites->AppendSeparator();

	menuMode->Append( MenuItem_Mode_Toggle, wxT("Toggle Play Mode\tF7") );
	#if defined(__WINDOWS__) || defined(__WXGTK__)
	  menuMode->AppendRadioItem( MenuItem_Mode_LoopList, wxT("Repeat Playlist") );
	  menuMode->AppendRadioItem( MenuItem_Mode_LoopSong, wxT("Repeat Song") );
	  menuMode->AppendRadioItem( MenuItem_Mode_Shuffle, wxT("Shuffle") );
	#else
	  menuMode->AppendCheckItem( MenuItem_Mode_LoopList, wxT("Repeat Playlist") );
	  menuMode->AppendCheckItem( MenuItem_Mode_LoopSong, wxT("Repeat Song") );
	  menuMode->AppendCheckItem( MenuItem_Mode_Shuffle, wxT("Shuffle") );
	#endif

	menuTools->Append( MenuItem_Tools_Settings, wxT("Settings...\tF10") );
	menuTools->AppendSeparator();
	menuTools->Append( MenuItem_Tools_ScrollSel, wxT("Go to Selected Item\tCtrl-Shift") );
	menuTools->Append( MenuItem_Tools_ScrollPlaying, wxT("Go to Playing Item\tCtrl-Alt") );
	menuTools->Append( MenuItem_Tools_SleepTimer, wxT("Sleep Timer...\tCtrl-T") );
	menuTools->Append( MenuItem_Tools_ResetInvalidFlags, wxT("Reset Invalid Flags\tCtrl-Alt-R") );
	menuTools->Append( MenuItem_Tools_DelMissing, wxT("Remove Missing Files...\tCtrl-Alt-M") );
	menuTools->Append( MenuItem_Tools_DelDuplicate, wxT("Remove Duplicate Files...\tCtrl-Alt-D") );
	menuTools->Append( MenuItem_Tools_ViewHistory, wxT("View Playback History...\tCtrl-Alt-H") );
	menuTools->Append( MenuItem_Tools_ClearHistory, wxT("Clear Playback History\tCtrl-Alt-C") );

	menuHelp->Append( MenuItem_Help_Contents, wxT("Xapl Help...\tF1") );
	menuHelp->AppendSeparator();
	menuHelp->Append( MenuItem_Help_CheckUpdate, wxT("Check for Update...") );
	menuHelp->Append( MenuItem_Help_ReportBug, wxT("Report a Bug...") );
	menuHelp->AppendSeparator();
	menuHelp->Append( MenuItem_Help_About, wxT("About...") );

	// Add to menus
	Append( menuFile, wxT("&File") );
	Append( menuView, wxT("&View") );
	Append( menuPlay, wxT("&Play") );
	Append( menuFavorites, wxT("F&avorites") );
	Append( menuMode, wxT("&Mode") );
	Append( menuTools, wxT("&Tools") );
	Append( menuHelp, wxT("&Help") );

	// Default values
	menuView->Check( MenuItem_View_TogglePlaylist, true );
	menuView->Check( MenuItem_View_PLFileName, true );
	menuMode->Check( MenuItem_Mode_LoopList, true );
}

void xaplMenuBar::SetSpaceAcceleratorEnabled( bool e )
{
	if ( e )
	{
#if defined(__WINDOWS__)
		RebuildAccelTable();
#elif defined(__WXGTK20__)
		delete menuPlay->m_accel;
		menuPlay->m_accel = storedAccel;
#endif
	}
	else
	{
#if defined(__WINDOWS__)
		m_accelTable = wxNullAcceleratorTable;
#elif defined(__WXGTK20__)
		storedAccel = menuPlay->m_accel;
		menuPlay->m_accel = gtk_accel_group_new();
#endif
	}
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
	if ( format == xaplAUTO_TOGGLE )
	{
		switch( currentPlaylistFormat )
		{
		case PLFORMAT_FILENAME:
			currentPlaylistFormat = PLFORMAT_FULLPATH;
			break;
		case PLFORMAT_FULLPATH:
			currentPlaylistFormat = PLFORMAT_EXTM3U;
			break;
		case PLFORMAT_EXTM3U:
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
		if ( format == PLFORMAT_FILENAME || format == PLFORMAT_FULLPATH || format == PLFORMAT_EXTM3U )
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

	case PLFORMAT_EXTM3U:
		menuView->Check( MenuItem_View_PLFileName, false );
		menuView->Check( MenuItem_View_PLFullPath, false );
		menuView->Check( MenuItem_View_PLCustomTags, true );
		break;

	default:
		break;
	}
}

int xaplMenuBar::TogglePlayMode( int mode )
{
	if ( mode == xaplAUTO_TOGGLE )
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

	return currentPlayMode;
}
