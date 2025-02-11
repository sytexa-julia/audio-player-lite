#ifndef __XAPL_APPLICATION_H
#define __XAPL_APPLICATION_H

#include <wx/wx.h>
#include <wx/snglinst.h>
#include <wx/fileconf.h>

#include "xaplConstants.h"
#include "xaplUtilities.h"
#include "xaplPlaylistCtrl.h"

class xaplServer;
class xaplPlaylistWindow;

typedef struct
{
	bool plVisible;
	wxSize windowSize;
	wxPoint windowPosition;
#ifdef __WINDOWS__
	long windowTransparency;
#endif
	bool windowMinimized;
	bool windowMaximized;
	bool searchVisible;
	wxString searchTerms;
	bool singleInstance;
	bool alwaysOnTop;
	bool minToTray;

	wxString prevMp3Assoc;
	bool mp3;
	wxString prevOggAssoc;
	bool ogg;
	wxString prevWavAssoc;
	bool wav;
	wxString prevWmaAssoc;
	bool wma;
	wxString prevAacAssoc;
	bool aac;
	wxString prevFlacAssoc;
	bool flac;

	long plFormat;
	long playMode;
	bool softMute;
	long volume;
	wxString loadedPlaylist;
	long selectedFile;
	wxString lastOpenedDir;
	bool autoDeleteInvalid;
		
	bool forceSoftwareMixing;
	long bufferSize;

	xaplPlaylistStyle plStyle;

	wxArrayString mediaDirs;
} xaplConfiguration;

class xaplApplication : 
	public wxApp
{
public:
	xaplConfiguration *GetConfig( void ) { return this->m_cfg; };
	void MakeSingleInstance( bool m );
	void SaveConfig( void );
	void Exit( void );

private:
	xaplConfiguration *m_cfg;
	wxFileConfig *m_cfgFile;

	wxSingleInstanceChecker *m_checker;
	xaplServer *m_ipcServer;
	bool m_ipcServerRunning;

	xaplPlaylistWindow *m_ui;
	wxObject m_dummy;

	~xaplApplication( void );

	virtual bool OnInit( void );
	virtual int OnExit( void );

	virtual int FilterEvent( wxEvent &evt );

	void LoadConfig( void );
};

DECLARE_APP( xaplApplication )

#endif
