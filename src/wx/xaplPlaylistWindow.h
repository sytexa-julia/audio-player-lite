#ifndef __XAPL_MAIN_WINDOW_H
#define __XAPL_MAIN_WINDOW_H

#include <cstring>

#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/gbsizer.h>
#include <wx/progdlg.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>

#include "../common/xPlaylistLib/XplPlaylistHelper.h"

#include "xaplMenuBar.h"
#include "xaplSearchUI.h"
#include "xaplControlUI.h"
#include "xaplConstants.h"
#include "xaplUtilities.h"
#include "xaplSleepTimer.h"
#include "xaplTaskBarIcon.h"
#include "xaplAboutDialog.h"
#include "xaplApplication.h"
#include "xaplTimeObserver.h"
#include "xaplHistoryDialog.h"
#include "xaplPlaylistCtrl.h"
#include "xaplPlaybackTimer.h"
#include "xaplSettingsDialog.h"
#include "xaplFileDropTarget.h"
#include "xaplFavoritesManager.h"
#include "xaplPropertiesDialog.h"
#include "xaplAudioEventListener.h"

#ifdef __WINDOWS__
  #include "resource.h"
#else
  #include "../../art/app32.xpm"
#endif

class xaplPlaylistWindow
	: public wxFrame, 
	  public xaplTimeObserver, 
	  public xaplAudioEventListener
{
	friend class xaplApplication;
	friend class xaplConnection;

private:
	// Functional Items
	xaplPlaybackTimer *positionTimer;
	xaplFavoritesManager *favoritesMgr;
	wxMenu *listContextMenu;
	wxMenu *addToFavorite;

	// Controls
	xaplSleepTimerUI *sleepTimer;
	xaplTaskBarIcon *taskBarIcon;
	
	xaplMenuBar *menuBar;
	xaplControlUI *controls;
	xaplPlaylistCtrl *playlist;
	xaplHistoryDialog *history;
	xaplSearchUI *search;
	wxStatusBar *statusBar;
	wxCommandEvent dummyCommand;

	// State Variables
	wxSize lastSize;
	long nextTrackId;
	bool loadingSettings;
	bool maximized;
	unsigned char prevVolume;

#ifdef __WINDOWS__
	long hotKeyHack;
#endif

protected:
	void Initialize( void );
	void ApplyConfig( void );
	void SaveConfig( void );
	void TogglePlaylist( bool showPL );

public:
	xaplPlaylistWindow( void );
	~xaplPlaylistWindow( void );
	void SetTempStatusText( const wxString &text );
	void RestoreTempStatusText( void );
	void UpdateSongCountStatusText( void );

	void OnPlayModeChanged( int playMode );

	// Window Events
	void OnResize( wxSizeEvent &evt );
	void OnMove( wxMoveEvent &evt );
	void OnClose( wxCloseEvent &evt );
	void OnIconize( wxIconizeEvent &evt );
	void OnMaximize( wxMaximizeEvent &evt );

	// General Events
	virtual void OnTrackChanged( xaplPlaylistItem *oldTrack, xaplPlaylistItem *newTrack );
	virtual void OnPlayingStatusChanged( int prevStatus, int status, xaplPlaylistItem *currentTrack = NULL );
	virtual void OnVolumeChanged( unsigned char oldVolume, unsigned char newVolume );
	virtual void OnPositionChanged( float oldPosition, float newPosition );

	virtual void OnTimeUpdated( void );

#ifdef __WINDOWS__
	void OnHotKey( wxKeyEvent &evt );
#endif
	void OnKeyDown( wxKeyEvent &evt );
	void OnPlaylistRightClick( wxMouseEvent &evt );
	void OnSearchBarHidden( bool itemChosen = false );

	// Control Events
	void OnPlaylistItemActivated( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonPlayPause( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonStop( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonPrevSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonNextSong( wxCommandEvent &WXUNUSED( evt ) );

	// Menu Events
	void OnMenuOpenFile( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuOpenLocation( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuAddInternetStream( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuClearPlaylist( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuNewPlaylist( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuSavePlaylist( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuOpenPlaylist( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuExit( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuTogglePlaylist( wxCommandEvent &evt );
	void OnMenuTogglePlaylistFormat( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewFilename( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewFullPath( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewM3U( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuSoftMute( wxCommandEvent &evt );
	void OnMenuVolumeUp( wxCommandEvent &evt );
	void OnMenuVolumeDown( wxCommandEvent &evt );
	void OnMenuPlay( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuPause( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuStop( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuNextSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuPrevSong( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuAddFavorite( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuManageFavorites( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuLoadFavorite( wxCommandEvent &evt );

	void OnMenuTogglePlayMode( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeRptList( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeRptSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeShuffle( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuSettings( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuScrollToSelection( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuScrollToPlaying( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuSleepTimer( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuResetInvalids( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuDeleteMissing( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuDeleteDuplicates( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewHistory( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuClearHistory( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuHelp( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuCheckUpdate( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuReportBug( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuAbout( wxCommandEvent &WXUNUSED( evt ) );

	// Context Menu Events
	void OnQueue( wxCommandEvent &WXUNUSED( evt ) );
	void OnRemove( wxCommandEvent &WXUNUSED( evt ) );
	void OnAddToFavorite( wxCommandEvent &evt );
	void OnAddToNewFavorite( wxCommandEvent &WXUNUSED( evt ) );
	void OnExplore( wxCommandEvent &WXUNUSED( evt ) );
	void OnProperties( wxCommandEvent &WXUNUSED( evt ) );
	void OnPlayingProperties( wxCommandEvent &WXUNUSED( evt ) );

	DECLARE_EVENT_TABLE();
};

#endif
