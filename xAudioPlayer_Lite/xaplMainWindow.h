#ifndef __XAPL_MAIN_WINDOW_H
#define __XAPL_MAIN_WINDOW_H

#include "xAudioInterface.h"
#include "xaiChannelEventListener.h"
#include <wx/wx.h>
#include <wx/fileconf.h>
#include "xaplMenuBar.h"
#include "xaplAboutDialog.h"
#include "xaplControlUI.h"
#include "xaplSearchUI.h"
#include "xaplPlaybackTimer.h"
#include "xaplTimeObserver.h"
#include "xaplPositionObserver.h"
#include "xaplVolumeObserver.h"
#include "xaplPlaylistCtrl.h"
#include "xaplPlaylistManager.h"

class xaplMainWindow :
	public wxFrame, public xaplVolumeObserver, public xaplTimeObserver, public xaplPositionObserver, public xaiChannelEventListener
{
private:
	// Functional Items
	xAudioInterface * const audioInterface;
	xaplPlaybackTimer *positionTimer;
	wxFileConfig *configuration;

	xaplPlaylistManager *plManager;

	// Controls
	xaplAboutDialog *aboutDialog;
	xaplMenuBar *menuBar;
	xaplSearchUI *search;
	wxStatusBar *statusBar;

	xaplControlUI *controls;
	xaplPlaylistCtrl *playlist;

	// State Variables
	float prevVolume;

protected:
	void Initialize( void );

	void LoadConfig( void );
	void SaveConfig( void );

	void TogglePlaylist( bool showPL );

	static void AudioInterfaceCallback( int theEvent );

public:
	xaplMainWindow( xAudioInterface *aIface, const wxPoint& pos );
	~xaplMainWindow( void );

	// General Events
	virtual void OnChannelEvent( int eventID );
	virtual void OnTimeUpdated( void );
	virtual void OnSongPositionChanged( float newPos );
	virtual void OnVolumeChanged( float newVol );
	void OnPlay( const wxString &audioFile, bool startPlay = true );

	void KeyDown( wxKeyEvent &evt );

	// Control Events
	void OnPlaylistItemActivated( wxListEvent &evt );
	void OnButtonPlayPause( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonStop( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonPrevSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnButtonNextSong( wxCommandEvent &WXUNUSED( evt ) );

	// Menu Events
	void OnMenuOpenFile( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuOpenLocation( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuExit( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuTogglePlaylist( wxCommandEvent &evt );
	void OnMenuTogglePlaylistFormat( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewFilename( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewFullPath( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuViewCustom( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuSoftMute( wxCommandEvent &evt );
	void OnMenuVolumeUp( wxCommandEvent &evt );
	void OnMenuVolumeDown( wxCommandEvent &evt );
	void OnMenuPlay( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuPause( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuStop( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuNextSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuPrevSong( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuTogglePlayMode( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeRptList( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeRptSong( wxCommandEvent &WXUNUSED( evt ) );
	void OnMenuModeShuffle( wxCommandEvent &WXUNUSED( evt ) );

	void OnMenuAbout( wxCommandEvent &WXUNUSED( evt ) );

	DECLARE_EVENT_TABLE();
};

enum MainControls
{
	ID_Playlist = 101
};

#endif
