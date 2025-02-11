#ifndef __XAPL_CONTROL_UI_H
#define __XAPL_CONTROL_UI_H

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/gbsizer.h>
#include <wx/statline.h>

#include "xaplAudio.h"
#include "xaplBitmaps.h"
#include "xaplUtilities.h"

class xaplControlUI 
	: public wxPanel
{
private:
	wxStaticText *lblNowPlaying;
	wxStaticText *lblVolPercent;

	wxSlider *slSongPosition;
	wxSlider *slVolume;

	wxStaticText *lblSongCurrTime;
	wxStaticText *lblSongTotalTime;
	wxStaticText *lblSongCountdown;
	wxStaticText *slash1, *slash2;

	wxBitmap *playIcon;  // These two are reused, so we need reference to them
	wxBitmap *pauseIcon; // See previous line
	wxBitmapButton *btnPrevSong;
	wxBitmapButton *btnPlayPause;
	wxBitmapButton *btnStop;
	wxBitmapButton *btnNextSong;

	#define ICON_PLAY 2
	#define ICON_PAUSE 3
	bool m_thumbActive;
	int m_currentPlayPauseIcon;
	int m_songLength;

protected:
	void Initialize( void );

public:
	// Constructor / Destructor
	xaplControlUI( wxWindow *parent );
	~xaplControlUI( void );

	// Events
	void OnPositionChanging( wxScrollEvent &evt );
	void OnPositionChange( wxScrollEvent &evt );
	void OnVolumeChange( wxScrollEvent &evt );

	// Misc.
	void SetSongLength( int seconds );
	void SetSongPosition( int currentSeconds, int totalSeconds );
	int GetSongPosition( void ) const;
	void SetVolume( int vol );
	int GetVolume( void ) const;
	void SetPlaying( void );
	void SetPaused( void );
	void TogglePlayPauseIcon( void );

	DECLARE_EVENT_TABLE();
};

enum ControlIDs
{
	ID_NowPlaying = 21,
	ID_SongPosition,
	ID_SongTotalTime,
	ID_SongCurrTime,
	ID_SongCountdown,

	ID_NextSongButton,
	ID_PrevSongButton,
	ID_PlayPauseButton,
	ID_StopButton,

	ID_VolPercent,
	ID_Volume
};

#endif
